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
		float a = angle / 2;
		float c = cos(a);
		float s = sin(a);
		//Assign the values
		setX(axis.getX() * s);
		setY(axis.getY() * s);
		setZ(axis.getZ() * s);
		setW(c);
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

	inline Matrix4f toRotationMatrix() const {
		//https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
		Matrix4f m;

		float i = getX();
		float j = getY();
		float k = getZ();
		float r = getW();

		m.set(0, 0, 1 - (2 * j * j) - (2 * k * k)); m.set(0, 1, 2 * (i * j - k * r)); m.set(0, 2, 2 * (i * k + j * r)); m.set(0, 3, 0);
		m.set(1, 0, 2 * (i * j + k * r)); m.set(1, 1, 1 - (2 * i * i) - (2 * k * k)); m.set(1, 2, 2 * (j * k - i * r)); m.set(1, 3, 0);
		m.set(2, 0, 2 * (i * k - j * r)); m.set(2, 1, 2 * (j * k + i * r)); m.set(2, 2, 1 - (2 * i * i) - (2 * j * j)); m.set(2, 3, 0);
		m.set(3, 0, 0); m.set(3, 1, 0); m.set(3, 2, 0); m.set(3, 3, 1);

		return m;
	}

	static Vector3f rotate(const Vector3f& vector, const Quaternion& quaternion) {
		//http://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		Vector3f u(quaternion.getX(), quaternion.getY(), quaternion.getZ());
		float s = quaternion.getW();

		return u * 2.0f * u.dot(vector) + vector * (s * s - u.dot(u)) + u.cross(vector) * 2.0f * s;
	}
};

#endif /* EXPERIMENTAL_QUATERNION_H_ */
