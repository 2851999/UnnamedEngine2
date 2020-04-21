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

#pragma once

#include <vector>
#include <string>

/*****************************************************************************
 * The Controller class
 *****************************************************************************/

class Controller {
private:
	/* The index of this controller for GLFW */
	int index;

	/* The name of this controller */
	std::string name;

	/*  The last states of the axes on the controller, and the number of
	 * axes */
	std::vector<float> axisValues;
	int                axisCount;

	/*  The last states of the buttons on the controller, and the number
	 * of buttons */
	std::vector<unsigned char> buttonValues;
	int                        buttonCount;

	const float*         currentAxisValues;
	const unsigned char* currentButtonValues;
public:
	/* The constructor */
	Controller(int index);

	/* The destructor */
	virtual ~Controller() {}

	/* The method used to check for any input from this controller */
	void checkInput();

	/* The getters */
	inline std::string getName() { return name; }
	inline int getIndex() { return index; }
	inline float getAxis(unsigned int index) { return axisValues[index]; }
	inline float getAxisCount() { return axisCount; }
	bool getButton(unsigned int index);
};

