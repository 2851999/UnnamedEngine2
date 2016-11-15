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

#include "Camera.h"
#include "../../utils/Utils.h"

/*****************************************************************************
 * The Camera2D class
 *****************************************************************************/

void Camera2D::update() {
	//Setup the view matrix
	getViewMatrix().setIdentity();
	getViewMatrix().transformR(getPosition(), getRotation(), getScale());
}

/*****************************************************************************
 * The Camera3D class
 *****************************************************************************/

void Camera3D::update() {
	//Calculate the forwards direction
//	Vector3f rotation = getRotationEuler();
//	cameraFront = Vector3f(
//		cos(MathsUtils::toRadians(rotation.getY())) * cos(MathsUtils::toRadians(rotation.getX())),
//		sin(MathsUtils::toRadians(rotation.getX())),
//		sin(MathsUtils::toRadians(rotation.getY())) * cos(MathsUtils::toRadians(rotation.getX()))
//	).normalise();
//	cameraRight = cameraFront.cross(cameraUp).normalise();
//	Vector3f up = cameraRight.cross(cameraFront).normalise();
	cameraFront = getTransform()->getRotation().getForward();
	cameraRight = getTransform()->getRotation().getRight();
	Vector3f up = getTransform()->getRotation().getUp();

	getViewMatrix().initLookAt(getPosition(), getPosition() + cameraFront, up);

//	frustum.setProjection(110, 1280.0f / 720.0f, 0.1f, 100.0f);
//	frustum.setView(getPosition(), getPosition() + cameraFront, up);

	//Update the SkyBox if there is one
	if (skyBox)
		skyBox->update(getPosition());
}
