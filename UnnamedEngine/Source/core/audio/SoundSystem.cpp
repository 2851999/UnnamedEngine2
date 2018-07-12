/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *****************************************************************************/

#include "SoundSystem.h"

#include <algorithm>

/***************************************************************************************************
 * The SoundSystem class
 ***************************************************************************************************/

void SoundSystem::addSequence(std::string key, std::vector<std::string>& sourceKeys, bool loop, float playInterval) {
	//The list of audio sources to use
	std::vector<AudioSource*> sequenceSources;
	//Assign the sources
	for (unsigned int i = 0; i < sourceKeys.size(); i++)
		sequenceSources.push_back(getSource(sourceKeys[i]));
	//Create and add the sequence
	sequences.insert(std::pair<std::string, AudioSequence*>(key, new AudioSequence(sequenceSources, loop, playInterval)));
}

void SoundSystem::play(std::string key) {
	AudioSource* source = getSource(key);
	source->play();
	playing.push_back(source);
}

void SoundSystem::pause(std::string key) {
	getSource(key)->pause();
}

void SoundSystem::resume(std::string key) {
	getSource(key)->resume();
}

void SoundSystem::stop(std::string key) {
	AudioSource* source = getSource(key);
	source->stop();
	playing.erase(std::remove(playing.begin(), playing.end(), source));
}

void SoundSystem::requestStop(std::string key) {
	AudioSource* source = getSource(key);
	source->requestStop();
}

void SoundSystem::playSequence(std::string key) {
	AudioSequence* sequence = getSequence(key);
	sequence->play();
	playingSequences.push_back(sequence);
}

void SoundSystem::pauseSequence(std::string key) {
	getSource(key)->pause();
}

void SoundSystem::resumeSequence(std::string key) {
	getSource(key)->resume();
}

void SoundSystem::stopSequence(std::string key) {
	AudioSequence* sequence = getSequence(key);
	sequence->stop();
	playingSequences.erase(std::remove(playingSequences.begin(), playingSequences.end(), sequence));
}

void SoundSystem::requestStopSequence(std::string key) {
	AudioSequence* sequence = getSequence(key);
	sequence->requestStop();
}

void SoundSystem::fadeIn(std::string key, float fadeTime) {
	AudioSource* source = getSource(key);
	source->fadeIn(fadeTime);
	playing.push_back(source);
}

void SoundSystem::fadeOut(std::string key, float fadeTime) {
	AudioSource* source = getSource(key);
	source->fadeOut(fadeTime);
}

void SoundSystem::pauseAll() {
	//Go through all playing audio
	for (unsigned int i = 0; i < playing.size(); i++)
		playing[i]->pause();
}

void SoundSystem::resumeAll() {
	//Go through all playing audio
	for (unsigned int i = 0; i < playing.size(); i++)
		playing[i]->resume();
}

void SoundSystem::stopAll() {
	//Go through all playing audio
	for (unsigned int i = 0; i < playing.size(); i++)
		playing[i]->stop();
	playing.clear();
}

void SoundSystem::update() {
	//Update the listener
	if (listener != NULL)
		listener->update();
	//Go through the sequences
	for (unsigned int a = 0; a < playingSequences.size(); a++) {
		//Check to see whether the sequence
		if (playingSequences.at(a)->isPlayingOrPaused())
			//Update the sequence
			playingSequences.at(a)->update();
		else
			playingSequences.erase(playingSequences.begin() + a);
	}
	//Go through the sources
	for (unsigned int a = 0; a < playing.size(); a++) {
		//Check to see whether the source is still playing
		if (playing.at(a)->isPlayingOrPaused())
			//Update the source
			playing.at(a)->update();
		else
			playing.erase(playing.begin() + a);
	}
}

void SoundSystem::destroy() {
	sources.clear();
	playing.clear();

	if (listener)
		delete listener;
}
