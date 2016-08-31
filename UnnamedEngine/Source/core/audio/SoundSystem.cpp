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

/***************************************************************************************************
 * The SoundSystem class
 ***************************************************************************************************/

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

void SoundSystem::update() {
	//Update the listener
	if (listener != NULL)
		listener->update();
	//Go through the sources
	for (unsigned int a = 0; a < playing.size(); a++) {
		//Check to see whether the source is still playing
		if (playing.at(a)->isPlaying())
			//Update the source
			playing.at(a)->update();
		else
			playing.erase(playing.begin() + a);
	}
}

void SoundSystem::destroy() {
	sources.clear();
	playing.clear();

	delete listener;
}
