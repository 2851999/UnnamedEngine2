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

#ifndef CORE_INPUT_CONTROLLER_H_
#define CORE_INPUT_CONTROLLER_H_

#include <vector>
#include <string>

/*****************************************************************************
 * The Controller class
 *****************************************************************************/

class Controller {
private:
	/* The index of this controller for GLFW */
	int index;

	/*  The last states of the axes on the controller, and the number of
	 * axes */
	std::vector<float> axisValues;
	int                axisCount;

	/*  The last states of the buttons on the controller, and the number
	 * of buttons */
	std::vector<unsigned char> buttonValues;
	int                        buttonCount;
public:
	/* The constructor */
	Controller(int index);

	/* The method used to check for any input from this controller */
	void checkInput();

	/* The getters */
	std::string getName();
	inline int getIndex() { return index; }
	inline float getAxis(int index) { return axisValues[index]; }
	inline float getAxisCount() { return axisCount; }
	bool getButton(int index);
};

#endif /* CORE_INPUT_CONTROLLER_H_ */
