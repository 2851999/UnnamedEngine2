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

#ifndef UTILS_TIMER_H_
#define UTILS_TIMER_H_

/***************************************************************************************************
 * The Timer class
 ***************************************************************************************************/

class Timer {
private:
	bool m_running;
	bool m_paused;
	bool m_stopped;
	long m_startTime;
	long m_endTime;
	long m_pauseStart;
	long m_pauseTotal;
public:
	Timer();
	void start();
	void pause();
	void resume();
	void stop();
	void reset();
	inline void restart() { reset(); start(); }
	long getTime();
	bool hasTimePassed(long time);
	inline bool hasTimePassedSeconds(float seconds) { return hasTimePassed((long) seconds * 1000); }
	inline bool hasTimePassedMinutes(float minutes) { return hasTimePassed((long) minutes * 1000 * 60); }
	inline bool hasTimePassedHours(float hours) { return hasTimePassed((long) hours * 1000 * 60 * 60); }
	inline float getSeconds() { return ((float) getTime()) / 1000.0f; }
	inline float getMinutes() { return ((float) getTime()) / 1000.0f / 60.0f; }
	inline float getHours() { return ((float) getTime()) / 1000.0f / 60.0f / 60.0f; }
	inline bool isRunning() { return m_running; }
	inline bool isPaused() { return m_paused; }
	inline bool isStopped() { return m_stopped; }
};

#endif /* UTILS_TIME_H_ */
