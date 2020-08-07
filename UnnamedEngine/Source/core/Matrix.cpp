/*****************************************************************************
 *
 *   Copyright 2018 Joel Davies
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

#include "Matrix.h"
#include "BaseEngine.h"

const Matrix4f& Matrix4f::initOrthographic(float left, float right, float bottom, float top, float zNear, float zFar) {
	set(0, 0, 2.0f / (right - left)); set(0, 1, 0);                     set(0, 2, 0);                      set(0, 3, -(right + left) / (right - left));
	set(1, 0, 0);                     set(1, 1, 2.0f / (top - bottom)); set(1, 2, 0);                      set(1, 3, -(top + bottom) / (top - bottom));
	if (!BaseEngine::usingVulkan()) {
		set(2, 0, 0);                     set(2, 1, 0);                     set(2, 2, -2.0f / (zFar - zNear)); set(2, 3, -(zFar + zNear) / (zFar - zNear));
	} else {
		set(2, 0, 0);                     set(2, 1, 0);                     set(2, 2, -1.0f / (zFar - zNear)); set(2, 3, -zNear / (zFar - zNear));
	}
	set(3, 0, 0);                     set(3, 1, 0);                     set(3, 2, 0);                      set(3, 3, 1);

	return (*this);
}

const Matrix4f& Matrix4f::initPerspective(float fovy, float aspect, float zNear, float zFar) {
	float scale = (tan((fovy / 2) * (utils_maths::PI / 180)));

	set(0, 0, 1.0f / (aspect * scale)); set(0, 1, 0); set(0, 2, 0); set(0, 3, 0);
	set(1, 0, 0); set(1, 1, 1.0f / scale); set(1, 2, 0); set(1, 3, 0);
	if (! BaseEngine::usingVulkan()) {
		set(2, 0, 0); set(2, 1, 0); set(2, 2, -(zFar + zNear) / (zFar - zNear)); set(2, 3, -(2 * zFar * zNear) / (zFar - zNear));
	} else {
		set(2, 0, 0); set(2, 1, 0); set(2, 2, zFar / (zNear - zFar)); set(2, 3,  -(zFar * zNear) / (zFar - zNear)); //Depth values need to be in range 0-1
	}
	set(3, 0, 0); set(3, 1, 0); set(3, 2, -1); set(3, 3, 0);

	return (*this);
}

const Matrix4f& Matrix4f::initLookAt(const Vector3f& eye, const Vector3f& centre, const Vector3f& up) {
	//EXPLANATION: http://stackoverflow.com/questions/21152556/an-inconsistency-in-my-understanding-of-the-glm-lookat-function

	Vector3f forward = (centre - eye).normalise();
	Vector3f u = up.normalised();
	Vector3f side = forward.cross(u).normalise();
	u = side.cross(forward);

	set(0, 0, side.getX());     set(0, 1, side.getY());     set(0, 2, side.getZ());     set(0, 3, -side.dot(eye));
	set(1, 0, u.getX());        set(1, 1, u.getY());        set(1, 2, u.getZ());        set(1, 3, -u.dot(eye));
	set(2, 0, -forward.getX()); set(2, 1, -forward.getY()); set(2, 2, -forward.getZ()); set(2, 3, forward.dot(eye));
	set(3, 0, 0);               set(3, 1, 0);               set(3, 2, 0);               set(3, 3, 1);

	return (*this);
}
