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
	Quaternion(const Vector<float, 4>& base) { setX(base[0]); setY(base[1]); setZ(base[2]); setW(base[3]); }
	Quaternion(const Vector4f& base) { setX(base.getX()); setY(base.getY()); setZ(base.getZ()); setW(base.getW()); }
	Quaternion(const Vector4<float>& base) { setX(base.getX()); setY(base.getY()); setZ(base.getZ()); setW(base.getW()); }

	Quaternion(Vector3f axis, float angle) { initFromAxisAngle(axis, angle); }
	Quaternion(const Vector3f& angles) { initFromEulerAngles(angles); }
	Quaternion(const Matrix4f& m) { initFromRotationMatrix(m); }

	/* Method that initialises this quaternion using an axis angle representation (angle is in degrees) */
	Quaternion& initFromAxisAngle(const Vector3f& axis, float angle);

	/* Method that initialises this quaternion using a set of euler angles (angles are in degrees) */
	Quaternion& initFromEulerAngles(const Vector3f& angles);

	/* Method that initialises this quaternion using a matrix */
	Quaternion& initFromRotationMatrix(const Matrix4f& m);

	/* Method used to assign this quaternions values to look at something */
	Quaternion& lookAt(const Vector3f& eye, const Vector3f& centre, const Vector3f& up);

	/* Various operations */
	inline Quaternion operator*(const Quaternion& other) const {
		Quaternion result;
		result[3] = (getW() * other.getW()) - (getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ());
		result[0] = (getX() * other.getW()) + (getW() * other.getX()) + (getY() * other.getZ()) - (getZ() * other.getY());
		result[1] = (getY() * other.getW()) + (getW() * other.getY()) + (getZ() * other.getX()) - (getX() * other.getZ());
		result[2] = (getZ() * other.getW()) + (getW() * other.getZ()) + (getX() * other.getY()) - (getY() * other.getX());

		return result;
	}

	inline Quaternion& operator*=(const Quaternion& other) {
		setW((getW() * other.getW()) - (getX() * other.getX()) - (getY() * other.getY()) - (getZ() * other.getZ()));
		setX((getX() * other.getW()) + (getW() * other.getX()) + (getY() * other.getZ()) - (getZ() * other.getY()));
		setY((getY() * other.getW()) + (getW() * other.getY()) + (getZ() * other.getX()) - (getX() * other.getZ()));
		setZ((getZ() * other.getW()) + (getW() * other.getZ()) + (getX() * other.getY()) - (getY() * other.getX()));

		return (*this);
	}

	inline Vector3f operator*(const Vector3f& other) const {
		return rotate(other, (*this));
	}

	inline Quaternion operator*(float value) const {
		Quaternion result;
		result[0] = getX() * value;
		result[1] = getY() * value;
		result[2] = getZ() * value;
		result[3] = getW() * value;
		return result;
	}

	static Quaternion slerp(const Quaternion& a, const Quaternion& b, float factor);

	inline Quaternion conjugate() const { return Quaternion(-getX(), -getY(), -getZ(), getW()); }

	Matrix4f toRotationMatrix();
	Vector3f toEuler();

	inline Vector3f getForward() const {
		return rotate(Vector3f(0.0f, 0.0f, -1.0f), (*this));
	}

	inline Vector3f getBackward() const {
		return rotate(Vector3f(0.0f, 0.0f, 1.0f), (*this));
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

	/* Static method to rotate a vector using a quaternion */
	static Vector3f rotate(const Vector3f& vector, const Quaternion& quaternion) {
		//http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		Vector3f u(quaternion.getX(), quaternion.getY(), quaternion.getZ());
		float s = quaternion.getW();

		return (u * 2.0f * u.dot(vector) + vector * (s * s - u.dot(u)) + u.cross(vector) * 2.0f * s);
	}
};

