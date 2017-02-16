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
	long  lastFrame = 0;
	long  currentDelta = 0; //(Delta is the time between two frames)
	float currentDeltaSeconds = 0;
	long  lastFPSCountUpdate = 0;
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

	/* The constructor */
	FPSCalculator(Mode mode = PER_SECOND) { this->mode = mode; }

	/* The destructor */
	virtual ~FPSCalculator() {}

	/* Method called to start - called to prevent initial delta being
	 * high/fps being low because of loading at startup */
	void start();

	/* The method used to update everything and recalculate the delta/fps
	 * if necessary */
	void update();

	/* Used to reset everything */
	void reset();

	/* Setters and getters */
	void setMode(Mode mode) { this->mode = mode; }

	Mode getMode() { return mode; }
	long getDelta() { return currentDelta; }
	float getDeltaSeconds() { return currentDeltaSeconds; }
	unsigned int getFPS() { return currentFPS; }
private:
	/* The mode to use when calculating the FPS */
	Mode mode;
};

/*****************************************************************************
 * The FPSLimiter class is used to limit the FPS by pausing the current thread
 *****************************************************************************/

class FPSLimiter {
private:
	/* The maximum allowed FPS */
	unsigned int maxFPS = 0;

	/* The time at the start of the current frame */
	long startOfFrame = 0;

	/* The delta when at the maximum FPS */
	long targetDelta = 0;
public:
	/* The constructor */
	FPSLimiter(unsigned int maxFPS = 0) { setMaxFPS(maxFPS); }

	/* The destructor */
	virtual ~FPSLimiter() {}

	/* Assigns the maximum FPS */
	void setMaxFPS(unsigned int maxFPS);

	/* Called at the start of a frame, to assign the time */
	void startFrame();

	/* Calculates the time to pause the thread for an then sleeps for that amount
	 * of time - should be called at the end of a frame */
	void endFrame();
};

#endif /* UTILS_FPSUTILS_H_ */
