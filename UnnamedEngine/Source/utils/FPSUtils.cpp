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

#include "FPSUtils.h"
#include "Utils.h"

using namespace TimeUtils;

/*****************************************************************************
 * The FPSCalculator class
 *****************************************************************************/

void FPSCalculator::start() {
	if (! started) {
		lastFrame = getMilliseconds();
		lastFPSCountUpdate = lastFrame;
		started = true;
	}
}

void FPSCalculator::update() {
	if (mode != OFF) {
		long current = getMilliseconds();

		currentDelta = current - lastFrame;
		lastFrame = current;

		if (mode == PER_FRAME) {
			if (currentDelta != 0)
				currentFPS = (int) (1000.0f / currentDelta);
		} else {
			fpsCount++;

			if (current - lastFPSCountUpdate >= 1000) {
				lastFPSCountUpdate = current;
				currentFPS = fpsCount;
				fpsCount = 0;
			}
		}
	}
}

void FPSCalculator::reset() {
	lastFrame = 0;
	currentDelta = 0;
	lastFPSCountUpdate = 0;
	fpsCount = 0;
	started = false;
	currentFPS = 0;
}

/*****************************************************************************
 * The FPSLimiter class
 *****************************************************************************/

void FPSLimiter::setMaxFPS(unsigned int maxFPS) {
	this->maxFPS = maxFPS;
	if (maxFPS != 0)
		targetDelta = (long) (1000.0f / (float) maxFPS);
}

void FPSLimiter::update(long currentDelta) {
	if (maxFPS > 0) {
		long difference = targetDelta - currentDelta;

		if (difference > 0)
			sleep(difference);
		else
			sleep(targetDelta);
	}
}
