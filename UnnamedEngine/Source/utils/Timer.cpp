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
	startTime = utils_time::getMilliseconds();
	state = State::RUNNING;
}

void Timer::pause() {
	pauseStart = utils_time::getMilliseconds();
	state = State::PAUSED;
}

void Timer::resume() {
	pauseTotal += utils_time::getMilliseconds() - pauseStart;
	state = State::RUNNING;
}

void Timer::stop() {
	endTime = utils_time::getMilliseconds();
	state = State::STOPPED;
}

void Timer::reset() {
	state = State::NONE;
	startTime = 0;
	endTime = 0;
	pauseStart = 0;
	pauseTotal = 0;
}

long Timer::getTime() {
	if (state == State::STOPPED)
		return endTime - startTime - pauseTotal;
	else if (state == State::PAUSED)
		return pauseStart - startTime - pauseTotal;
	else if (state == State::RUNNING)
		return utils_time::getMilliseconds() - startTime - pauseTotal;
	else
		return 0;
}

void Timer::setTime(long time) {
	if (state == State::STOPPED)
		endTime = startTime + pauseTotal + time;
	else if (state == State::PAUSED)
		pauseStart = startTime + pauseTotal + time;
	else if (state == State::RUNNING)
		startTime = utils_time::getMilliseconds() - pauseTotal - time;
}

bool Timer::hasTimePassed(long time) {
	if (state != State::NONE)
		return getTime() >= time;
	else
		return false;
}
