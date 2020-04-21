/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "DebugProfiler.h"

#include "Logging.h"

 /*****************************************************************************
  * The DebugProfiler class
  *****************************************************************************/

void DebugProfiler::start(float timeToProfile, std::function<void(Result)> callback) {
	this->timeToProfile = timeToProfile;
	this->callback = callback;

	//Clear all data
	samplesFPS.clear();
	result = Result();

	timer.start();
}

void DebugProfiler::stop() {
	timer.stop();

	//Compute the results of profiling
	for (unsigned int fps : samplesFPS) {
		result.fpsMin = utils_maths::min(result.fpsMin, fps);
		result.fpsMax = utils_maths::max(result.fpsMax, fps);
		result.fpsAvg += (float) fps;
	}
	result.fpsAvg /= samplesFPS.size();

	//Call the callback
	callback(result);
}

void DebugProfiler::update(unsigned int fps) {
	if (timer.isRunning()) {
		samplesFPS.push_back(fps);

		if (timer.hasTimePassedSeconds(timeToProfile))
			stop();
	}
}

void DebugProfiler::printResult(Result result) {
	std::string output = "Min FPS: " + utils_string::str(result.fpsMin) + ", Max FPS: " + utils_string::str(result.fpsMax) + ", Average FPS: " + utils_string::str(result.fpsAvg);

	Logger::log(output, "DebugProfiler", LogType::Debug);
}