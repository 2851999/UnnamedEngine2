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

#ifndef CORE_QUATERNION_H_
#define CORE_QUATERNION_H_

#include "Vector.h"

class Matrix4f;

/*****************************************************************************
 * The Quaternion class used to representing quaternions
 *****************************************************************************/

class Quaternion : public Vector4f {
public:
	/* The constructors */
	Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) {
		setX(x); setY(y); setZ(z); setW(w);
	}
	Quaternion(const Vector<float, 4> base) { setX(base[0]); setY(base[1]); setZ(base[2]); setW(base[3]); }
	Quaternion(const Vector4f& base) { setX(base.getX()); setY(base.getY()); setZ(base.getZ()); setW(base.getW()); }
	Quaternion(const Vector4<float>& base) { setX(base.getX()); setY(base.getY()); setZ(base.getZ()); setW(base.getW()); }

	Quaternion(Vector3f axis, float angle) {
		//Calculate some needed constants for transforming it into the
		//quaternion representation
		float a = MathsUtils::toRadians(angle / 2);
		float s = sinf(a);
		float c = cosf(a);
		//Assign the values
		setX(axis.getX() * s);
		setY(axis.getY() * s);
		setZ(axis.getZ() * s);
		setW(c);
	}

	Quaternion(Vector3f euler);

	/* Various operations */
	inline Quaternion operator*(const Quaternion& other) const {
		Quaternion result;

		result[0] = (getW() * other.getX()) + (getX() * other.getW()) - (getY() * other.getZ()) + (getZ() * other.getY());
		result[1] = (getW() * other.getY()) + (getX() * other.getZ()) + (getY() * other.getW()) - (getZ() * other.getX());
		result[2] = (getW() * other.getZ()) - (getX() * other.getY()) + (getY() * other.getX()) + (getZ() * other.getW());
		result[3] = (getW() * other.getW()) - (getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());

		return result;
	}

	inline Quaternion& operator*=(const Quaternion& other) {
		float x = (getW() * other.getX()) + (getX() * other.getW()) - (getY() * other.getZ()) + (getZ() * other.getY());
		float y = (getW() * other.getY()) + (getX() * other.getZ()) + (getY() * other.getW()) - (getZ() * other.getX());
		float z = (getW() * other.getZ()) - (getX() * other.getY()) + (getY() * other.getX()) + (getZ() * other.getW());
		float w = (getW() * other.getW()) - (getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());

//		float w = (getW() * other.getW()) - (getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());
//		float x = (getX() * other.getW()) + (getW() * other.getX()) + (getY() * other.getZ()) - (getZ() * other.getY());
//		float y = (getY() * other.getW()) + (getW() * other.getY()) + (getZ() * other.getX()) - (getX() * other.getZ());
//		float z = (getZ() * other.getW()) + (getW() * other.getZ()) + (getX() * other.getY()) - (getY() * other.getX());

		setX(x);
		setY(y);
		setZ(z);
		setW(w);

		return (*this);
	}

	inline Quaternion operator*(const Vector3f& other) const {
		Quaternion result;

		result[0] =  (getW() * other.getX()) + (getY() * other.getZ()) - (getZ() * other.getY());
		result[1] =  (getW() * other.getY()) + (getZ() * other.getX()) - (getX() * other.getZ());
		result[2] =  (getW() * other.getZ()) + (getX() * other.getY()) - (getY() * other.getX());
		result[3] = -(getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());

		return result;
	}

	inline Quaternion& operator*=(const Vector3f& other) {
		float x =  (getW() * other.getX()) + (getY() * other.getZ()) - (getZ() * other.getY());
		float y =  (getW() * other.getY()) + (getZ() * other.getX()) - (getX() * other.getZ());
		float z =  (getW() * other.getZ()) + (getX() * other.getY()) - (getY() * other.getX());
		float w = -(getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());

		setX(x);
		setY(y);
		setZ(z);
		setW(w);

		return (*this);
	}

	inline Quaternion conjugate() const { return Quaternion(-getX(), -getY(), -getZ(), getW()); }

	Matrix4f toRotationMatrix();

	Vector3f toEuler();

	inline Vector3f getForward() const {
		return rotate(Vector3f(0.0f, 0.0f, 1.0f), (*this));
	}

	inline Vector3f getBackward() const {
		return rotate(Vector3f(0.0f, 0.0f, -1.0f), (*this));
	}

	inline Vector3f getUp() const {
		return rotate(Vector3f(0.0f, 1.0f, 0.0f), (*this));
	}

	inline Vector3f getDown() const {
		return rotate(Vector3f(0.0f, -1.0f, 0.0f), (*this));
	}

	inline Vector3f getLeft() const {
		return rotate(Vector3f(-1.0f, 0.0f, 0.0f), (*this));
	}

	inline Vector3f getRight() const {
		return rotate(Vector3f(1.0f, 0.0f, 0.0f), (*this));
	}

	static Vector3f rotate(const Vector3f& vector, const Quaternion& quaternion) {
		//http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		Vector3f u(quaternion.getX(), quaternion.getY(), quaternion.getZ());
		float s = quaternion.getW();

		return u * 2.0f * u.dot(vector) + vector * (s * s - u.dot(u)) + u.cross(vector) * 2.0f * s;
	}
};

#endif /* CORE_QUATERNION_H_ */
