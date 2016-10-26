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

#include "Timer.h"
#include "Utils.h"

/*****************************************************************************
 * The Timer class
 *****************************************************************************/

Timer::Timer() {
	reset();
}

void Timer::start() {
	startTime = TimeUtils::getMilliseconds();
	running = true;
	stopped = false;
}

void Timer::pause() {
	pauseStart = TimeUtils::getMilliseconds();
	paused = true;
}

void Timer::resume() {
	pauseTotal += TimeUtils::getMilliseconds() - pauseStart;
	paused = false;
}

void Timer::stop() {
	endTime = TimeUtils::getMilliseconds();
	running = false;
	stopped = true;
}

void Timer::reset() {
	running = false;
	paused = false;
	stopped = false;
	startTime = 0;
	endTime = 0;
	pauseStart = 0;
	pauseTotal = 0;
}

long Timer::getTime() {
	if (stopped)
		return endTime - startTime - pauseTotal;
	else if (paused)
		return pauseStart - startTime - pauseTotal;
	else
		return TimeUtils::getMilliseconds() - startTime - pauseTotal;
}

bool Timer::hasTimePassed(long time) {
	if (running || paused || stopped)
		return getTime() >= time;
	else
		return false;
}
