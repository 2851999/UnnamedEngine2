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

#ifndef UTILS_FPSUTILS_H_
#define UTILS_FPSUTILS_H_

/*****************************************************************************
 * The FPSCalculator class is used to keep track of the current frame rate
 *****************************************************************************/

class FPSCalculator {
private:
	/* Time data, measured in milliseconds */
	long lastFrame = 0;
	long currentDelta = 0;
	long lastFPSCountUpdate = 0;
	/* The current FPS count used in PER_SECOND */
	unsigned int fpsCount = 0;
	bool started = false;
	/* The current FPS */
	unsigned int currentFPS = 0;
public:
	/* Modes for calculating the FPS */
	enum Mode {
		/* Don't calculate the FPS */
		OFF,
		/* Calculate the FPS based on the current delta between frames */
		PER_FRAME,
		/* Calculate the FPS by counting how many frames occur in a second */
		PER_SECOND
	};

	FPSCalculator(Mode mode = PER_SECOND) { this->mode = mode; }
	virtual ~FPSCalculator() {}

	void start();

	void update();
	void reset();

	void setMode(Mode mode) { this->mode = mode; }
	Mode getMode() { return mode; }
	long getDelta() { return currentDelta; }
	unsigned int getFPS() { return currentFPS; }
private:
	Mode mode;
};

/*****************************************************************************
 * The FPSLimiter class is used to limit the FPS by pausing the current thread
 *****************************************************************************/

class FPSLimiter {
private:
	unsigned int maxFPS = 0;
	long targetDelta = 0;

public:
	FPSLimiter(unsigned int maxFPS = 0) { setMaxFPS(maxFPS); }
	virtual ~FPSLimiter() {}

	/* Assigns the maximum FPS */
	void setMaxFPS(unsigned int maxFPS);

	/* Calculates the time to pause the thread for an then sleeps for that amount
	 * of time */
	void update(long currentDeta);
};

#endif /* UTILS_FPSUTILS_H_ */
