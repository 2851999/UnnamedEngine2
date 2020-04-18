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

#include "../core/input/InputBindings.h"
#include "../core/render/Camera.h"

/*****************************************************************************
 * The DebugCamera class helps to create a basic camera with free movement
 *****************************************************************************/

class DebugCamera : public Camera3D, public InputListener {
private:
	/* Input stuff */
	InputBindings* inputBindings = NULL;

	InputBindingAxis* axis0 = NULL; //Forwards/Backwards
	InputBindingAxis* axis1 = NULL; //Left/Right
	InputBindingAxis* axis2 = NULL; //Look X
	InputBindingAxis* axis3 = NULL; //Look Y

	/* Movement speed */
	float movementSpeed;

	/* Mouse sensitivity */
	float mouseSensitivityX;
	float mouseSensitivityY;

	/* Look sensitivity (For other controllers) */
	float lookSensitivityX;
	float lookSensitivityY;

	/* The maximum x rotation angle */
	float maxRotationX;

	/* The current delta (When update() was last called) */
	float currentDelta = 0;
public:
	/* The constructors */
	DebugCamera(Matrix4f projectionMatrix, InputBindings* bindings = NULL);
	DebugCamera(float fovy, float aspect, float zNear, float zFar, InputBindings* bindings = NULL) : DebugCamera(Matrix4f().initPerspective(fovy, aspect, zNear, zFar), bindings) {}

	/* The destructor */
	virtual ~DebugCamera() { delete inputBindings; }

	/* Method used to update this camera */
	void update(float delta);

	/* Called when the mouse moves */
	virtual void onMouseMoved(double x, double y, double dx, double dy) override;

	/* Setters and getters */
	InputBindingAxis* getForwardsAxis() { return axis0; }
	InputBindingAxis* getSidewaysAxis() { return axis1; }
	InputBindingAxis* getLookXAxis() { return axis2; }
	InputBindingAxis* getLookYAxis() { return axis3; }
	InputBindings* getInputBindings() { return inputBindings; }

	inline void setMovementSpeed(float speed) { this->movementSpeed = speed; }
	inline void setMouseSensitivityX(float x) { this->mouseSensitivityX = x; }
	inline void setMouseSensitivityY(float y) { this->mouseSensitivityY = y; }
	inline void setMouseSensitivity(float x, float y) { setMouseSensitivityX(x); setMouseSensitivityY(y); }
	inline void setLookSensitivityX(float x) { this->lookSensitivityX = x; }
	inline void setLookSensitivityY(float y) { this->lookSensitivityY = y; }
	inline void setLookSensitivity(float x, float y) { setLookSensitivityX(x); setLookSensitivityY(y); }
	inline void setMaxRotationX(float angle) { this->maxRotationX = angle; }

	inline float getMovementSpeed() { return movementSpeed; }
	inline float getMouseSensitivityX() { return mouseSensitivityX; }
	inline float getMouseSensitivityY() { return mouseSensitivityY; }
	inline float getLookSensitivityX() { return lookSensitivityX; }
	inline float getLookSensitivityY() { return lookSensitivityY; }
	inline float getMaxRotationX() { return maxRotationX; }


};

