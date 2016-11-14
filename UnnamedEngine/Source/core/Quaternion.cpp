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

Quaternion::Quaternion(Vector3f euler) {
	float c1 = cos(MathsUtils::toRadians(euler.getX())/2);
	float s1 = sin(MathsUtils::toRadians(euler.getX())/2);
	float c2 = cos(MathsUtils::toRadians(euler.getY())/2);
	float s2 = sin(MathsUtils::toRadians(euler.getY())/2);
	float c3 = cos(MathsUtils::toRadians(euler.getZ())/2);
	float s3 = sin(MathsUtils::toRadians(euler.getZ())/2);


	setW(c1*c2*c3-s1*s2*s3);
	setX(s1*c2*c3+c1*s2*s3);
	setY(c1*s2*c3-s1*c2*s3);
	setZ(c1*c2*s3+s1*s2*c3);

//	float hx = MathsUtils::toRadians(euler.getX()) / 2.0f;
//	float hy = MathsUtils::toRadians(euler.getY()) / 2.0f;
//	float hz = MathsUtils::toRadians(euler.getZ()) / 2.0f;
//	float c1 = cos(hy);
//	float s1 = sin(hy);
//	float c2 = cos(hx);
//	float s2 = sin(hx);
//	float c3 = cos(hz);
//	float s3 = sin(hz);
//	float c1c2 = c1 * c2;
//	float s1s2 = s1 * s2;
//	setX(c1 * s2 * c3 - s1 * c2 * s3);
//	setY(s1 * c2 * c3 + c1 * s2 * s3);
//	setZ(c1c2 * s3 + s1s2 * c3);
//	setW(c1c2 * c3 - s1s2 * s3);

//	float t0 = cos(hy);
//	float t1 = sin(hy);
//	float t2 = cos(hz);
//	float t3 = sin(hz);
//	float t4 = cos(hx);
//	float t5 = cos(hx);
//	setW(t0 * t2 * t4 + t1 * t3 * t5);
//	setX(t0 * t3 * t4 - t1 * t2 * t5);
//	setY(t0 * t2 * t5 + t1 * t3 * t4);
//	setZ(t1 * t2 * t4 - t0 * t3 * t5);
//
//	Quaternion test = Quaternion(Vector3f(1.0f, 0.0f, 0.0f), MathsUtils::toRadians(euler.getX())) * Quaternion(Vector3f(0.0f, 1.0f, 0.0f), MathsUtils::toRadians(euler.getY())) * Quaternion(Vector3f(0.0f, 0.0f, 1.0f), MathsUtils::toRadians(euler.getZ()));
//	setX(test.getX());
//	setY(test.getY());
//	setZ(test.getZ());
//	setW(test.getW());
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
	m.set(3, 0, 0);                       m.set(3, 1, 0);                       m.set(3, 2, 0);                       m.set(3, 3, 1);

	return m;
}

Vector3f Quaternion::toEuler() {
//	float x = MathsUtils::toDegrees(asin(2 * getX() * getY() + 2 * getZ() * getW()));
//	float y = MathsUtils::toDegrees(atan2(2 * getY() * getW() - 2 * getX() * getZ(), 1 - 2 * getY() * getY() - 2 * getZ() * getZ()));
//	float z = MathsUtils::toDegrees(atan2(2 * getX() * getW() - 2 * getY() * getZ(), 1 - 2 * getX() * getX() - 2 * getZ() * getZ()));
//	return Vector3f(x, y, z);

//	float test = getX() * getY() + getZ() * getW();
//	float x, y, z;
//	if (test > 0.499) { // singularity at north pole
//		y = 2 * atan2(getX(), getW());
//		x = MathsUtils::PI / 2;
//		z = 0;
//		return Vector3f(MathsUtils::toDegrees(x), MathsUtils::toDegrees(y), MathsUtils::toDegrees(z));
//	}
//	if (test < -0.499) { // singularity at south pole
//		y = -2 * atan2(getX(), getW());
//		x = -MathsUtils::PI / 2;
//		z = 0;
//		return Vector3f(MathsUtils::toDegrees(x), MathsUtils::toDegrees(y), MathsUtils::toDegrees(z));
//	}
//    float sqx = getX() * getX();
//    float sqy = getY() * getY();
//    float sqz = getZ() * getZ();
//    y = atan2(2.0f * getY() * getW() -2.0f * getX() * getZ(), 1.0f - 2.0f * sqy - 2.0f * sqz);
//	x = asin(2.0f * test);
//	z = atan2(2.0f * getX() * getW() -2.0f * getY() * getZ(), 1.0f - 2.0f * sqx - 2.0f * sqz);
//	return Vector3f(MathsUtils::toDegrees(x), MathsUtils::toDegrees(y), MathsUtils::toDegrees(z));

	float eX = atan2(-2 * (getY() * getZ()-getW() * getX()), getW() * getW()-getX() * getX()-getY() * getY()+getZ() * getZ());
	float eY = asin(2 * (getX() * getZ() + getW() * getY()));
	float eZ = atan2(-2 * (getX() * getY()-getW() * getZ()), getW() * getW()+getX() * getX()-getY() * getY()-getZ() * getZ());
	return Vector3f(MathsUtils::toDegrees(eX), MathsUtils::toDegrees(eY), MathsUtils::toDegrees(eZ));
}
