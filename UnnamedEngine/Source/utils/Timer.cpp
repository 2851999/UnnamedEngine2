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

#include "Utils.h"
#include "Timer.h"

/***************************************************************************************************
 * The Timer class
 ***************************************************************************************************/

Timer::Timer() {
	reset();
}

void Timer::start() {
	m_startTime = TimeUtils::getMilliseconds();
	m_running = true;
	m_stopped = false;
}

void Timer::pause() {
	m_pauseStart = TimeUtils::getMilliseconds();
	m_paused = true;
}

void Timer::resume() {
	m_pauseTotal += TimeUtils::getMilliseconds() - m_pauseStart;
	m_paused = false;
}

void Timer::stop() {
	m_endTime = TimeUtils::getMilliseconds();
	m_running = false;
	m_stopped = true;
}

void Timer::reset() {
	m_running = false;
	m_paused = false;
	m_stopped = false;
	m_startTime = 0;
	m_endTime = 0;
	m_pauseStart = 0;
	m_pauseTotal = 0;
}

long Timer::getTime() {
	if (m_stopped)
		return m_endTime - m_startTime - m_pauseTotal;
	else if (m_paused)
		return m_pauseStart - m_startTime - m_pauseTotal;
	else
		return TimeUtils::getMilliseconds() - m_startTime - m_pauseTotal;
}

bool Timer::hasTimePassed(long time) {
	if (m_running || m_paused || m_stopped)
		return getTime() >= time;
	else
		return false;
}
