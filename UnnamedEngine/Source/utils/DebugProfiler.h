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

#include "Utils.h"
#include "Timer.h"

#include <fstream>
#include <functional>

 /*****************************************************************************
  * The DebugProfiler class
  *****************************************************************************/

class DebugProfiler {
public:
	/* Structure of computed data from the last time profiling occurred */
	struct Result {
		unsigned int fpsMin = 10000;
		unsigned int fpsMax = 0;
		float fpsAvg;
	};
private:
	/* Callback for when this profiler finishes profiling */
	std::function<void(Result)> callback;

	/* Timer to time how often samples should be taken */
	Timer timer;

	/* States how often samples should be taken */
	float timeBetweenSamples;

	/* Amount of time to profile for before stopping (in seconds) */
	float timeToProfile;

	/* The samples taken of fps*/
	std::vector<unsigned int> samplesFPS;

	/* Computed data from the last time profiling occurred */
	Result result;
public:
	/* Constructor */
	DebugProfiler() {}

	/* Destructor */
	virtual ~DebugProfiler() {}

	/* Method to start profiling given the time to perform it, and a callback for
	   when it is complete */
	void start(float timeToProfile, std::function<void(Result)> callback);

	/* Method to stop profiling */
	void stop();

	/* Method to update this profiler and stop if necessary */
	void update(unsigned int fps);

	/* Method called to print out the result */
	static void printResult(Result result);
};
