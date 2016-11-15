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
	//Ensure it hasn't already been started
	if (! started) {
		//Assign the last frame's time
		lastFrame = getMilliseconds();
		lastFPSCountUpdate = lastFrame;
		currentDelta = 0;
		currentDeltaSeconds = 0;
		started = true;
	}
}

void FPSCalculator::update() {
	//Only calculate everything if necessary
	if (mode != OFF) {
		//Obtain the current time in milliseconds
		long current = getMilliseconds();

		//With a high FPS the delta will become 0, but to stop this causing issues
		//the current delta will only be assigned when this is not the case and
		//some time has passed
		if (current != lastFrame) {
			//Calculate the current time in milliseconds
			currentDelta = current - lastFrame;
			currentDeltaSeconds = (float) currentDelta / 1000.0f;

			//Assign the last frame time
			lastFrame = current;
		}

		//Check the mode
		if (mode == PER_FRAME) {
			//Prevent divide by 0
			if (currentDelta != 0)
				//Calculate the FPS as the number of frames completed if the delta
				//remained constant for 1000 milliseconds
				currentFPS = (int) (1000.0f / currentDelta);
		} else {
			//Increment the frame counter
			fpsCount++;

			//Check whether it has been 1 second since the last FPS update
			if (current - lastFPSCountUpdate >= 1000) {
				//It has, so update the current FPS
				lastFPSCountUpdate = current;
				currentFPS = fpsCount;
				fpsCount = 0;
			}
		}
	}
}

void FPSCalculator::reset() {
	//Reset all of the variables
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
	//Assign the maximum FPS
	this->maxFPS = maxFPS;
	//Prevent a divide by 0
	if (maxFPS != 0)
		//Calculate the target delta to achieve the desired frame rate
		targetDelta = (long) (1000.0f / (float) maxFPS);
}

void FPSLimiter::startFrame() {
	//Assign the time of the start of the current frame
	startOfFrame = TimeUtils::getMilliseconds();
}

void FPSLimiter::endFrame() {
	//Ensure there is a frame cap
	if (maxFPS > 0) {
		//Calculate the difference in the target delta, and the current one
		long difference = targetDelta - (TimeUtils::getMilliseconds() - startOfFrame);

		//Pause the thread for the time necessary
		if (difference > 0)
			sleep(difference);
	}
}
