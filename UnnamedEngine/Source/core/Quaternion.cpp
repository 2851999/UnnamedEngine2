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

#include "Quaternion.h"

#include "Matrix.h"

/*****************************************************************************
 * The Quaternion class
 *****************************************************************************/

Quaternion& Quaternion::initFromAxisAngle(const Vector3f& axis, float angle) {
	//Calculate some needed constants for transforming it into the
	//quaternion representation
	float a = utils_maths::toRadians(angle / 2.0f);
	float s = sin(a);
	float c = cos(a);
	//Assign the values
	setX(axis.getX() * s);
	setY(axis.getY() * s);
	setZ(axis.getZ() * s);
	setW(c);

	return (*this);
}

Quaternion& Quaternion::initFromEulerAngles(const Vector3f& angles) {
	float h = utils_maths::toRadians(angles.getY());
	float a = utils_maths::toRadians(angles.getZ());
	float b = utils_maths::toRadians(angles.getX());

    float c1 = cosf(h / 2.0f);
    float s1 = sinf(h / 2.0f);
    float c2 = cosf(a / 2.0f);
    float s2 = sinf(a / 2.0f);
    float c3 = cosf(b / 2.0f);
    float s3 = sinf(b / 2.0f);
    float c1c2 = c1 * c2;
    float s1s2 = s1 * s2;
    setW(c1c2 * c3 - s1s2 * s3);
  	setX(c1c2 * s3 + s1s2 * c3);
	setY(s1 * c2 * c3 + c1 * s2 * s3);
	setZ(c1 * s2 * c3 - s1 * c2 * s3);

	return (*this);
}

Quaternion& Quaternion::initFromRotationMatrix(const Matrix4f& m) {
	float trace = m.get(0, 0) + m.get(1, 1) + m.get(2, 2);

	if (trace > 0) {
		float s = 0.5f / sqrtf(trace + 1.0f);
		setW(0.25f / s);
		setX((m.get(2, 1) - m.get(1, 2)) * s);
		setY((m.get(0, 2) - m.get(2, 0)) * s);
		setZ((m.get(1, 0) - m.get(1, 0)) * s);
	} else if (m.get(0, 0) > m.get(1, 1) && m.get(0, 0) > m.get(2, 2)) {
		float s = 2.0f * sqrtf(1.0f + m.get(0, 0) - m.get(1, 1) - m.get(2, 2));
		setW((m.get(2, 1) - m.get(1, 2)) / s);
		setX(0.25f * s);
		setY((m.get(0, 1) + m.get(1, 0)) / s);
		setZ((m.get(0, 2) + m.get(2, 0)) / s);
	} else if (m.get(1, 1) > m.get(2, 2)) {
		float s = 2.0f * sqrtf(1.0f + m.get(1, 1) - m.get(0, 0) - m.get(2, 2));
		setW((m.get(0, 2) - m.get(2, 0)) / s);
		setX((m.get(0, 1) + m.get(1, 0)) / s);
		setY(0.25f * s);
		setZ((m.get(1, 2) + m.get(2, 1)) / s);
	} else {
		float s = 2.0f * sqrtf(1.0f + m.get(2, 2) - m.get(1, 1) - m.get(0, 0));
		setW((m.get(1, 0) - m.get(0, 1)) / s);
		setX((m.get(0, 2) + m.get(2, 0)) / s);
		setY((m.get(2, 1) + m.get(2, 1)) / s);
		setZ(0.25f * s);
	}
	normalise();

	return (*this);
}

Quaternion& Quaternion::lookAt(const Vector3f& eye, const Vector3f& centre, const Vector3f& up) {
	return initFromRotationMatrix(Matrix4f().initLookAt(eye, centre, up));
}

Matrix4f Quaternion::toRotationMatrix() {
	//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
	Matrix4f m;

	float i = getX();
	float j = getY();
	float k = getZ();
	float r = getW();

	float ii = i * i;
	float ij = i * j;
	float ik = i * k;
	float ir = i * r;
	float jj = j * j;
	float jk = j * k;
	float jr = j * r;
	float kk = k * k;
	float kr = k * r;

	m.set(0, 0, 1 - (2 * jj) - (2 * kk)); m.set(0, 1, 2 * (ij - kr));           m.set(0, 2, 2 * (ik + jr));           m.set(0, 3, 0);
	m.set(1, 0, 2 * (ij + kr));           m.set(1, 1, 1 - (2 * ii) - (2 * kk)); m.set(1, 2, 2 * (jk - ir));           m.set(1, 3, 0);
	m.set(2, 0, 2 * (ik - jr));           m.set(2, 1, 2 * (jk + ir));           m.set(2, 2, 1 - (2 * ii) - (2 * jj)); m.set(2, 3, 0);
	m.set(3, 0, 0);                       m.set(3, 1, 0);                       m.set(3, 2, 0);                       m.set(3, 3, 1);                       m.set(3, 3, 1);

	return m;
}

Vector3f Quaternion::toEuler() {
    float sqw = getW() * getW();
    float sqx = getX() * getX();
    float sqy = getY() * getY();
    float sqz = getZ() * getZ();
	float unit = sqx + sqy + sqz + sqw;
	float test = getX() * getY() + getZ() * getW();
	float h, a, b;
	if (test > 0.499f * unit) {
		h = 2 * atan2f(getX(),getW());
		a = utils_maths::PI / 2.0f;
		b = 0;
	} else if (test < -0.499f * unit) {
		h = -2 * atan2f(getX(),getW());
		a = -utils_maths::PI / 2.0f;
		b = 0;
	} else {
	    h = atan2f(2 * getY() * getW() -2.0f * getX() * getZ(), sqx - sqy - sqz + sqw);
		a = asinf(2 * test / unit);
		b = atan2f(2 * getX() * getW() -2.0f * getY() * getZ() , -sqx + sqy - sqz + sqw);
	}
	return Vector3f(utils_maths::toDegrees(b), utils_maths::toDegrees(h), utils_maths::toDegrees(a));
}
