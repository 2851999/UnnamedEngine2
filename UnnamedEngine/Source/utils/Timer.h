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

/*****************************************************************************
 * The Timer class
 *****************************************************************************/

class Timer {
private:
	enum State {
		NONE,
		RUNNING,
		PAUSED,
		STOPPED
	};

	/* The current timer state */
	State state;

	/* The start and end times of this timer */
	long startTime;
	long endTime;

	/* The start time of the last pause, and the total time that this timer
	 * has been paused for */
	long pauseStart;
	long pauseTotal;
public:
	/* The constructor and destructors */
	Timer();
	virtual ~Timer() {}

	/* Various timer related methods */
	void start();
	void pause();
	void resume();
	void stop();
	void reset();
	inline void restart() { reset(); start(); }

	/* Returns the current time in milliseconds */
	long getTime();

	/* Various methods to check whether a certain amount of time has passed
	 * according to this timer */
	bool hasTimePassed(long time);
	inline bool hasTimePassedSeconds(float seconds) { return hasTimePassed((long) seconds * 1000); }
	inline bool hasTimePassedMinutes(float minutes) { return hasTimePassed((long) minutes * 1000 * 60); }
	inline bool hasTimePassedHours(float hours) { return hasTimePassed((long) hours * 1000 * 60 * 60); }

	/* Various other methods to get the current time in other formats */
	inline float getSeconds() { return ((float) getTime()) / 1000.0f; }
	inline float getMinutes() { return ((float) getTime()) / 1000.0f / 60.0f; }
	inline float getHours()   { return ((float) getTime()) / 1000.0f / 60.0f / 60.0f; }

	/* Various methods to check the current state of this timer */
	inline bool isRunning() { return state == State::RUNNING; }
	inline bool isPaused()  { return state == State::PAUSED; }
	inline bool isStopped() { return state == State::STOPPED; } //Note: Will return false if timer was never started (since last reset)
};

#endif /* UTILS_TIME_H_ */
