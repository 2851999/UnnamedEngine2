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

	return data;
}

AudioData* AudioLoader::loadOggFile(std::string path) {
	//Create the audio data object
	AudioData* data = new AudioData();

	//This will store the number of channels in the audio file
	int numChannels;

	//Read the file and assign the size of it in bytes
	data->size = stb_vorbis_decode_filename(path.c_str(), &numChannels, &data->frequency, &data->data) * sizeof(short);

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
		gain = ((float) Window::getCurrentInstance()->getSettings().audioSoundEffectVolume) / 100.0f;
	else if (type == TYPE_MUSIC)
		gain = ((float) Window::getCurrentInstance()->getSettings().audioMusicVolume) / 100.0f;
	else {
		gain = 1.0f;
	}
}

void AudioSource::update() {
	if (AudioManager::hasContext()) {
		//Get all of the needed values
		Vector3f sourcePosition = getPosition();
		//Vector3f sourceRotation = this.getRotation();
		Vector3f sourceVelocity = getVelocity();
		//Update all of the source values
		alSource3f(sourceHandle, AL_POSITION, sourcePosition.getX(), sourcePosition.getY(), sourcePosition.getZ());
		alSource3f(sourceHandle, AL_VELOCITY, sourceVelocity.getX(), sourceVelocity.getY(), sourceVelocity.getZ());
		alSourcef(sourceHandle, AL_PITCH, pitch);
		alSourcef(sourceHandle, AL_GAIN, gain);
		if (loop)
			alSourcei(sourceHandle, AL_LOOPING, AL_TRUE);
		else
			alSourcei(sourceHandle, AL_LOOPING, AL_FALSE);
	}
}

void AudioSource::play() {
	if (AudioManager::hasContext())
		alSourcePlay(sourceHandle);
}

void AudioSource::stop() {
	if (AudioManager::hasContext())
		alSourceStop(sourceHandle);
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
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
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
