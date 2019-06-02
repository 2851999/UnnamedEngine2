/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *****************************************************************************/

#include "Audio.h"

#include <algorithm>
#include <fstream>

#define _MSC_VER
#include <stb/stb_vorbis.h>

#include "../Window.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The AudioLoader class
 *****************************************************************************/

AudioData* AudioLoader::loadFile(std::string path) {
	//Check the file type based on the file extension
	if (utils_string::strEndsWith(path, ".wav"))
		return loadWaveFile(path);
	else if (utils_string::strEndsWith(path, ".ogg"))
		return loadOggFile(path);
	else {
		Logger::log("Unknown audio file type of file '" + path + "'", "AudioLoader", LogType::Error);
		return NULL;
	}
}

AudioData* AudioLoader::loadWaveFile(std::string path) {
	//The file input stream
	std::ifstream input;

	//Structures to store the data from the file
	Wave_RIFF	 waveRiff;
	Wave_fmt	waveFmt;
	Wave_data	 waveData;

	//The audio data
	AudioData* data = NULL;

	//Open the file
	input.open(path.c_str(), std::ios::binary);

	//Ensure the file was opened
	if (input.is_open()) {
		//Read the RIFF data
		input.read((char*) &waveRiff, sizeof(Wave_RIFF));

		//Ensure the RIFF header data is correct for a wave file
		if (waveRiff.ChunkID[0] != 'R' || waveRiff.ChunkID[1] != 'I' || waveRiff.ChunkID[2] != 'F' || waveRiff.ChunkID[3] != 'F') {
			input.close();
			Logger::log("Wave file '" + path + "' has an invalid 'RIFF' header", "AudioLoader", LogType::Error);
			return NULL;
		}

		if (waveRiff.Format[0] != 'W' || waveRiff.Format[1] != 'A' || waveRiff.Format[2] != 'V' || waveRiff.Format[3] != 'E') {
			input.close();
			Logger::log("Wave file '" + path + "' has an invalid 'RIFF' header for a wave file", "AudioLoader", LogType::Error);
			return NULL;
		}

		//Read the 'fmt' sub-chunk
		input.read((char*) &waveFmt, sizeof(Wave_fmt));

		//Ensure it is valid
		if (waveFmt.SubChunkID[0] != 'f' || waveFmt.SubChunkID[1] != 'm' || waveFmt.SubChunkID[2] != 't' || waveFmt.SubChunkID[3] != ' ') {
			input.close();
			Logger::log("Wave file '" + path + "' has an invalid 'fmt' header", "AudioLoader", LogType::Error);
			return NULL;
		}

		//Read the first section of audio data
		input.read((char*) &waveData, sizeof(Wave_data));

		//Ensure the data is valid
		if (waveData.SubChunkID[0] != 'd' || waveData.SubChunkID[1] != 'a' || waveData.SubChunkID[2] != 't' || waveData.SubChunkID[3] != 'a') {
			input.close();
			Logger::log("Wave file '" + path + "' has an invalid 'data' header", "AudioLoader", LogType::Error);
			return NULL;
		}

		//Create the AudioData instance and create the data array
		data = new AudioData();
		data->data = new short[waveData.SubChunkSize];

		//Read all of the remaining audio data
		input.read((char*) data->data, waveData.SubChunkSize);

		//Setup the rest of the AudioData instance
		data->size = waveData.SubChunkSize;
		data->frequency = waveFmt.SampleRate;

		if (waveFmt.NumChannels == 1) {
			if (waveFmt.BitsPerSample == 8)
				data->format = AL_FORMAT_MONO8;
			else if (waveFmt.BitsPerSample == 16)
				data->format = AL_FORMAT_MONO16;
		} else if (waveFmt.NumChannels == 2) {
			if (waveFmt.BitsPerSample == 8)
				data->format = AL_FORMAT_STEREO8;
			else if (waveFmt.BitsPerSample == 16)
				data->format = AL_FORMAT_STEREO16;
		}

		//Close the input file
		input.close();
	} else
		//An error occurred
		Logger::log("Wave file '" + path + "' could not be opened successfully", "AudioLoader", LogType::Error);

	return data;
}

AudioData* AudioLoader::loadOggFile(std::string path) {
	//Create the audio data object
	AudioData* data = new AudioData();

	//This will store the number of channels in the audio file
	int numChannels;

	//Read the file and assign the size of it in bytes
	data->size = stb_vorbis_decode_filename(path.c_str(), &numChannels, &data->frequency, &data->data) * sizeof(short);
	data->size *= numChannels;

	//Check if the data was not read properly
	if (data->size < 0) {
		Logger::log("Ogg Vorbis file '" + path + "' was not read successfully", "AudioLoader", LogType::Error);
		delete data;
		return NULL;
	}

	//Assign the format
	if (numChannels == 1)
		data->format = AL_FORMAT_MONO16;
	else if (numChannels == 2)
		data->format = AL_FORMAT_STEREO16;

	return data;
}

/***************************************************************************************************
 * The AudioSource class
 ***************************************************************************************************/

AudioSource::AudioSource(AudioData* data, unsigned int type) {
	//Ensure there is an audio context before attempting to setup the audio
	if (AudioManager::hasContext()) {
		this->type = type;
		alGenSources(1, &sourceHandle);
		alGenBuffers(1, &bufferHandle);
		pitch = 1.0f;
		updateVolume();
		loop = false;

		alBufferData(bufferHandle, data->format, (void*) data->data, data->size, data->frequency);
		alSourcei(sourceHandle, AL_BUFFER, bufferHandle);

		AudioManager::add(this);
	}
}

void AudioSource::updateVolume() {
	if (type == TYPE_SOUND_EFFECT)
		gain = volumeFraction * ((float) Window::getCurrentInstance()->getSettings().audioSoundEffectVolume) / 100.0f;
	else if (type == TYPE_MUSIC)
		gain = volumeFraction * ((float) Window::getCurrentInstance()->getSettings().audioMusicVolume) / 100.0f;
	else {
		//Leave the volume as it is
	}
}

void AudioSource::update() {
	if (AudioManager::hasContext()) {
		//Ensure the audio should not wait for something
		if (timeStopped == -1.0f) {
			//Update the the position if necessary
			if (transform->hasChanged()) {
				//Get the position and pass it to OpenAL
				Vector3f sourcePosition = getPosition();
				alSource3f(sourceHandle, AL_POSITION, sourcePosition.getX(), sourcePosition.getY(), sourcePosition.getZ());
			}

			//Get all of the needed values
			//Vector3f sourceRotation = this.getRotation();
			Vector3f sourceVelocity = getVelocity();
			//Update the other of the source values
			alSource3f(sourceHandle, AL_VELOCITY, sourceVelocity.getX(), sourceVelocity.getY(), sourceVelocity.getZ());
			alSourcef(sourceHandle, AL_PITCH, pitch);
			alSourcef(sourceHandle, AL_GAIN, volumeFraction2 * gain);
			alSourcei(sourceHandle, AL_LOOPING, loop && loopInterval == 0.0f && ! requestedStop ? AL_TRUE : AL_FALSE);

			//Check if the audio has stopped playing
			if (! isPlaying() && isPlayingOrPaused()) {
				//Audio has finished

				//Check if the the audio should be restarted
				if (loop && loopInterval > 0.0f && ! requestedStop) {
					//The loop should be restarted after the loop interval

					//Assign the time stopped
					timeStopped = utils_time::getSeconds();
				} else
					//Sound should stop
					stop();
			}

			//Assign the volume fraction 2 if fading
			if (fadeState != -1) {
				float timeElapsed = utils_time::getSeconds() - fadeStart;

				if (fadeState == 1)
					volumeFraction2 = timeElapsed / fadeTime;
				else if (fadeState == 2)
					volumeFraction2 = fadeInitialVolF2 * (1.0f - (timeElapsed / fadeTime));

				//Ensure it is in the correct range
				volumeFraction2 = utils_maths::clamp(volumeFraction2, 0.0f, 1.0f);
				//Check if fading should stop
				if (timeElapsed >= fadeTime) {
					//Stop playing if it was fading out
					if (fadeState == 2)
						stop();
					else
						//Stop the fading
						fadeState = -1.0f;
				}
			} else
				volumeFraction2 = 1.0f;

		} else {
			//Ensure the sound shouldn't stop
			if (! requestedStop) {
				//The audio is waiting to restart, so check if the time is up
				if (utils_time::getSeconds() - timeStopped >= loopInterval)
					//Restart the audio
					play();
			} else
				//Stop the sound
				stop();
		}
	}
}

void AudioSource::play() {
	if (AudioManager::hasContext()) {
		alSourcePlay(sourceHandle);
		playingPaused = true;
		timeStopped = -1.0f;
		requestedStop = false;
	}
}

void AudioSource::stop() {
	if (AudioManager::hasContext()) {
		alSourceStop(sourceHandle);
		playingPaused = false;
		timeStopped = -1.0f;
		requestedStop = false;
		fadeState = -1;
	}
}

void AudioSource::pause() {
	if (AudioManager::hasContext())
		alSourcePause(sourceHandle);
}

void AudioSource::resume() {
	if (AudioManager::hasContext())
		alSourcePlay(sourceHandle);
}

void AudioSource::destroy() {
	if (AudioManager::hasContext()) {
		if (isPlaying())
			stop();

		alDeleteSources(1, &sourceHandle);
		alDeleteBuffers(1, &bufferHandle);
	}
}

void AudioSource::fadeIn(float fadeTime) {
	//Ensure not already fading in
	if (fadeState != 1) {
		this->fadeTime = fadeTime;
		fadeStart = utils_time::getSeconds();
		fadeState = 1;
		//Start with no sound
		volumeFraction2 = 0.0f;
		alSourcef(sourceHandle, AL_GAIN, 0.0f);

		play();
	}
}

void AudioSource::fadeOut(float fadeTime) {
	//Ensure not already fading out
	if (fadeState != 2) {
		this->fadeTime = fadeTime;
		fadeStart = utils_time::getSeconds();

		//Check if this was previously fading out
		if (fadeState == 1) {
			fadeInitialVolF2 = volumeFraction2;

			//Check if need to subtract some fade time
			if (volumeFraction2 < 1.0f) {
				//Subtract the amount of time it would have taken to go from 1.0 down
				//to whatever it currently is
				this->fadeTime -= (1.0f - volumeFraction2) * fadeTime;
			}
		} else
			fadeInitialVolF2 = 1.0f;

		fadeState = 2;
	}
}

bool AudioSource::isPlaying() {
	int value = 0;
	alGetSourcei(sourceHandle, AL_SOURCE_STATE, &value);
	return value == AL_PLAYING;
}

bool AudioSource::isPaused() {
	int value = 0;
	alGetSourcei(sourceHandle, AL_SOURCE_STATE, &value);
	return value == AL_PAUSED;
}

/***************************************************************************************************
 * The AudioSequence class
 ***************************************************************************************************/

void AudioSequence::update() {
	//Check if this sequence is playing
	if (currentIndex != -1) {
		//Ensure the audio should not wait for something
		if (timeStopped == -1) {
			//Check if the current source has finished
			if (! sources[currentIndex]->isPlaying()) {
				//Stop the source
				sources[currentIndex]->stop();
				//Check if stop is requested
				if (requestedStop) {
					//Stop this sequence
					currentIndex = -1;
					stop();
				} else {
					//Move on to the next (if there is one)
					if ((unsigned int) currentIndex < sources.size() - 1) {
						timeStopped = utils_time::getSeconds();
					} else {
						//Check if looping
						if (loop) {
							timeStopped = utils_time::getSeconds();
						} else {
							//Stop this sequence
							currentIndex = -1;
							stop();
						}
					}
				}
			} else {
				//Update the source
				sources[currentIndex]->update();
			}
		} else {
			//Check if its time to start the next source
			if (utils_time::getSeconds() - timeStopped >= playInterval) {
				if ((unsigned int) currentIndex < sources.size() - 1)
					currentIndex++;
				else
					currentIndex = 0;

				sources[currentIndex]->play();
				timeStopped = -1.0f;
			}
		}
	}
}

void AudioSequence::play() {
	currentIndex = 0;
	sources[currentIndex]->play();
	timeStopped = -1.0f;
	playingPaused = true;
	requestedStop = false;
}

void AudioSequence::pause() {
	if (currentIndex != -1)
		sources[currentIndex]->pause();
}

void AudioSequence::resume() {
	if (currentIndex != -1)
		sources[currentIndex]->resume();
}

void AudioSequence::stop() {
	if (currentIndex != -1)
		sources[currentIndex]->stop();
	timeStopped = -1.0f;
	playingPaused = false;
	requestedStop = false;
}

void AudioSequence::requestStop() {
	if (currentIndex != -1)
		sources[currentIndex]->requestStop();
	requestedStop = true;
}

void AudioSequence::fadeIn(float fadeTime) {
	currentIndex = 0;
	sources[currentIndex]->fadeIn(fadeTime);
	timeStopped = -1.0f;
	playingPaused = true;
	requestedStop = false;
}

void AudioSequence::fadeOut(float fadeTime) {
	if (currentIndex != -1)
		sources[currentIndex]->fadeOut(fadeTime);
}

/***************************************************************************************************
 * The AudioListener class
 ***************************************************************************************************/

void AudioListener::update() {
	if (AudioManager::hasContext()) {
		//Get the needed values
		Vector3f listenerPosition = getPosition();
		Vector3f listenerVelocity = getVelocity();
		Vector3f at = getRotation().getForward();
		Vector3f up = getRotation().getUp();

		//Setup OpenAL
		alListener3f(AL_POSITION, listenerPosition.getX(), listenerPosition.getY(), listenerPosition.getZ());
		alListener3f(AL_VELOCITY, listenerVelocity.getX(), listenerVelocity.getY(), listenerVelocity.getZ());
		ALfloat listenerOrientation[] = { at.getX(), at.getY(), at.getZ(), up.getX(), up.getY(), up.getZ() };
		alListenerfv(AL_ORIENTATION, listenerOrientation);
	}
}

/***************************************************************************************************
 * The Audio class
 ***************************************************************************************************/

std::vector<AudioSource*> AudioManager::sources;
ALCdevice* AudioManager::device;
ALCcontext* AudioManager::context;

void AudioManager::add(AudioSource* source) {
	sources.push_back(source);
}

void AudioManager::remove(AudioSource* source) {
	sources.erase(std::remove(sources.begin(), sources.end(), source), sources.end());
}


void AudioManager::initialise() {
	device = alcOpenDevice(NULL);
	if (device) {
		context = alcCreateContext(device, NULL);
		alcMakeContextCurrent(context);
	}
}

void AudioManager::destroy() {
	for (unsigned int a = 0; a < sources.size(); a++)
		delete sources.at(a);
	sources.clear();

	alcCloseDevice(device);
	alcDestroyContext(context);
}

void AudioManager::updateVolume() {
	for (unsigned int a = 0; a < sources.size(); a++)
		sources.at(a)->updateVolume();
}
