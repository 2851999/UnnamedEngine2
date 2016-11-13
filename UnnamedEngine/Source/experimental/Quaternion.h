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

#ifndef EXPERIMENTAL_QUATERNION_H_
#define EXPERIMENTAL_QUATERNION_H_

#include "../core/Vector.h"
class Matrix4f;

/*****************************************************************************
 * The Quaternion class used to representing quaternions
 *****************************************************************************/

class Quaternion : public Vector4f {
public:
	/* The constructors */
	Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) {
		setX(x); setY(y); setZ(z); setW(w);
	}

	Quaternion(Vector3f axis, float angle) {
		//Calculate some needed constants for transforming it into the
		//quaternion representation
		float a = MathsUtils::toRadians(angle) / 2;
		float s = sin(a);
		float c = cos(a);
		//Assign the values
		setX(axis.getX() * s);
		setY(axis.getY() * s);
		setZ(axis.getZ() * s);
		setW(c);
	}

	Quaternion(Vector3f euler) {
		float hx = MathsUtils::toRadians(euler.getX()) / 2;
		float hy = MathsUtils::toRadians(euler.getY()) / 2;
		float hz = MathsUtils::toRadians(euler.getZ()) / 2;
		float c1 = cos(hy);
		float s1 = sin(hy);
		float c2 = cos(hx);
		float s2 = sin(hx);
		float c3 = cos(hz);
		float s3 = sin(hz);
		float c1c2 = c1 * c2;
		float s1s2 = s1 * s2;
		setX(c1 * s2 * c3 - s1 * c2 * s3);
		setY(s1 * c2 * c3 + c1 * s2 * s3);
		setZ(c1c2 * s3 + s1s2 * c3);
		setW(c1c2 * c3 - s1s2 * s3);
	}

	/* Various operations */
	inline Quaternion operator*(const Quaternion& other) const {
		Quaternion result;
		result[0] = (getW() * other.getX()) + (getX() * other.getW()) - (getY() * other.getZ()) + (getZ() * other.getY());
		result[1] = (getW() * other.getY()) + (getX() * other.getZ()) + (getY() * other.getW()) - (getZ() * other.getX());
		result[2] = (getW() * other.getZ()) - (getX() * other.getY()) + (getY() * other.getX()) + (getZ() * other.getW());
		result[3] = (getW() * other.getW()) - (getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());
		return result;
	}

	inline Quaternion operator*(const Vector3f& other) const {
		Quaternion result;

		result[0] =  (getW() * other.getX()) + (getY() * other.getZ()) - (getZ() * other.getY());
		result[1] =  (getW() * other.getY()) + (getZ() * other.getX()) - (getX() * other.getZ());
		result[2] =  (getW() * other.getZ()) + (getX() * other.getY()) - (getY() * other.getX());
		result[3] = -(getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());

		return result;
	}

	inline Quaternion conjugate() const { return Quaternion(-getX(), -getY(), -getZ(), getW()); }

	Matrix4f toRotationMatrix();

	inline Vector3f toEuler() {
		float x = MathsUtils::toDegrees(atan2(2 * getX() * getW() - 2 * getY() * getZ(), 1 - 2 * getX() * getX() - 2 * getZ() * getZ()));
		float z = MathsUtils::toDegrees(asin(2 * getX() * getY() + 2 * getZ() * getW()));
		float y = MathsUtils::toDegrees(atan2(2 * getY() * getW() - 2 * getX() * getZ(), 1 - 2 * getY() * getY() - 2 * getZ() * getZ()));
		return Vector3f(x, y, z);
	}

	static Vector3f rotate(const Vector3f& vector, const Quaternion& quaternion) {
		//http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		Vector3f u(quaternion.getX(), quaternion.getY(), quaternion.getZ());
		float s = quaternion.getW();

		return u * 2.0f * u.dot(vector) + vector * (s * s - u.dot(u)) + u.cross(vector) * 2.0f * s;
	}
};

#endif /* EXPERIMENTAL_QUATERNION_H_ */
