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

#include "DebugCamera.h"
#include "../core/Window.h"

/*****************************************************************************
 * The DebugCamera class
 *****************************************************************************/

DebugCamera::DebugCamera(Matrix4f projectionMatrix, InputBindings* bindings) : Camera3D(projectionMatrix) {
	//Check whether the bindings need to be created
	if (! bindings) {
		//Create the input bindings
		inputBindings = new InputBindings();
		//Create the axis bindings
		axis0 = inputBindings->createAxisBinding("Axis0");
		axis1 = inputBindings->createAxisBinding("Axis1");
		axis2 = inputBindings->createAxisBinding("Axis2");
		axis3 = inputBindings->createAxisBinding("Axis3");

		//Setup default keys
		axis0->assignKeys(GLFW_KEY_W, GLFW_KEY_S);
		axis1->assignKeys(GLFW_KEY_A, GLFW_KEY_D);
	} else {
		//Use the existing one
		inputBindings = bindings;

		//Assign the axes
		axis0 = inputBindings->getAxisBinding("Axis0");
		axis1 = inputBindings->getAxisBinding("Axis1");
		axis2 = inputBindings->getAxisBinding("Axis2");
		axis3 = inputBindings->getAxisBinding("Axis3");
	}

	//Assign the default movement speed
	movementSpeed = 5.0f;

	//Assign the default mouse sensitivity values
	mouseSensitivityX = 10.0f;
	mouseSensitivityY = 10.0f;

	//Assign the default look sensitivity values
	lookSensitivityX = 100.0f;
	lookSensitivityY = 100.0f;

	//Assign the default max x rotation
	maxRotationX = 89.0f;

	//Add this input listener
	Window::getCurrentInstance()->getInputManager()->addListener(this);
}

void DebugCamera::update(float delta) {
	//Assign the current delta
	currentDelta = delta;
	//Move the camera
	moveForward(axis0->getValue() * currentDelta * movementSpeed);
	moveLeft(axis1->getValue() * currentDelta * movementSpeed);

	//Orientate the camera
	getRelRotation() += Vector3f(axis2->getValue() * lookSensitivityX, -axis3->getValue() * lookSensitivityY, 0) * currentDelta;
	getRelRotation().setX(MathsUtils::clamp(getRotation().getX(), -maxRotationX, maxRotationX));

	//Update the camera
	Camera3D::update();
}

void DebugCamera::onMouseMoved(double x, double y, double dx, double dy) {
	//Orientate the camera
	getRelRotation() += Vector3f(-dy * mouseSensitivityX, dx * mouseSensitivityY, 0) * currentDelta;
}

