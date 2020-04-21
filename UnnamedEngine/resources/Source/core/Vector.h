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

#include "../utils/Utils.h"

/*****************************************************************************
 * The generalised version of a Vector
 *****************************************************************************/

/* T - type of value, N - the number of values to store */
template<typename T, unsigned int N>
class Vector {
private:
	T values[N];
public:
	Vector() {}

	/* Various operations */
	inline T operator[](int i) const { return values[i]; }
	inline T& operator[](int i) { return values[i]; }

	/* Returns the result of adding another vector to this one */
	inline Vector<T, N> operator+(const Vector<T, N>& other) const {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] + other[i];
		return result;
	}

	/* Returns the result of subtracting another vector from this one */
	inline Vector<T, N> operator-(const Vector<T, N>& other) const {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] - other[i];
		return result;
	}

	/* Returns the result of multiplying this vector by another */
	inline Vector<T, N> operator*(const Vector<T, N>& other) const {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] * other[i];
		return result;
	}

	/* Returns the result of dividing this vector by another */
	inline Vector<T, N> operator/(const Vector<T, N>& other) const {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] / other[i];
		return result;
	}

	/* Adds another vector to this one */
	inline Vector<T, N>& operator+=(const Vector<T, N>& other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] += other[i];
		return (*this);
	}

	/* Subtracts another vector from this one */
	inline Vector<T, N>& operator-=(const Vector<T, N>& other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] -= other[i];
		return (*this);
	}

	/* Multiplies this vector by another one */
	inline Vector<T, N>& operator*=(const Vector<T, N>& other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] *= other[i];
		return (*this);
	}

	/* Divides this vector by another */
	inline Vector<T, N>& operator/=(const Vector<T, N>& other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] /= other[i];
		return (*this);
	}

	/* Returns the result of multiplying this vector by a scalar */
	inline Vector<T, N> operator*(const T& value) const {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] * value;
		return result;
	}

	/* Returns the result of dividing this vector by a scalar */
	inline Vector<T, N> operator/(const T& value) const {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] / value;
		return result;
	}

	/* Multiplies this vector by a scalar */
	inline Vector<T, N>& operator*=(const T& value) {
		for (unsigned int i = 0; i < N; i++)
			values[i] *= value;
		return (*this);
	}

	/* Divides this vector by a scalar */
	inline Vector<T, N>& operator/=(const T& value) {
		for (unsigned int i = 0; i < N; i++)
			values /= value;
		return (*this);
	}

	/* Compares this vector to another one and returns whether they are equal */
	inline bool operator==(const Vector<T, N>& other) {
		//Go through each value
		for (unsigned int i = 0; i < N; i++) {
			//Return false if it isn't equal
			if (other[i] != values[i])
				return false;
		}
		//Will only reach here and return true if every value compared were
		//equal to each other
		return true;
	}

	/* Various other comparison operators */
	inline bool operator!=(const Vector<T, N>& other) { return ! ((*this) == other);       }
	inline bool operator<(const Vector<T, N>& other)  { return length() < other.length();  }
	inline bool operator<=(const Vector<T, N>& other) { return length() <= other.length(); }
	inline bool operator>(const Vector<T, N>& other)  { return length() > other.length();  }
	inline bool operator>=(const Vector<T, N>& other) { return length() >= other.length(); }

	/* Method to linearly interpolate between two vectors */
	inline static Vector<T, N> lerp(const Vector<T, N>& a, const Vector<T, N>& b, T factor) { return (a + ((b - a) * factor)); }
	inline static Vector<T, N> slerp(const Vector<T, N>& a, const Vector<T, N>& b, T factor) {
		//https://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/
		float dot = a.dot(b);
		dot = utils_maths::clamp(dot, -1.0f, 1.0f);
		float theta = acosf(dot) * factor;
		Vector<T, N> relative = b - a * dot;
		relative.normalise();
		return ((a * cosf(theta)) + (relative * sinf(theta)));
	}
	inline static Vector<T, N> nlerp(const Vector<T, N>& a, const Vector<T, N>& b, T factor) { return lerp(a, b, factor).normalise(); }

	/* Finds the total length of the vector */
	inline T length() const {
		double total = 0;
		//Go through and add on the square of the current value to the total
		for (unsigned int i = 0; i < N; i++)
			total += (values[i] * values[i]);
		//Square root the total to find the length
		return sqrt(total);
	}

	/* Returns the unit vector (while modifying this one) */
	inline Vector<T, N> normalise() {
		//Calculate the length of this vector
		T length = this->length();
		if (length != 0) {
			//Go through each value and divide it by the length of this vector
			for (unsigned int i = 0; i < N; i++)
				values[i] /= length;
		}
		return (*this);
	}

	/* Returns the unit vector */
	inline Vector<T, N> normalised() const {
		Vector<T, N> result = (*this);
		return result.normalise();
	}

	/* Finds and returns the smallest component of this vector */
	inline T min() {
		if (N > 0) {
			//Get the first value
			T current = values[0];
			//Go through each value in this vector
			for (unsigned int i = 1; i < N; i++) {
				//Assign the current (lowest) value if it is less than the one
				//already there
				if (values[i] < current)
					current = values[i];
			}
			//Return the result
			return current;
		} else
			return 0;
	}

	/* Finds and returns the biggest component of this vector */
	inline T max() {
		if (N > 0) {
			//Get the first value
			T current = values[0];
			//Go through each value in this vector
			for (unsigned int i = 1; i < N; i++) {
				//Assign the current (highest) value if it is greater than the one
				//already there
				if (values[i] > current)
					current = values[i];
			}
			//Return the result
			return current;
		} else
			return 0;
	}

	/* Returns the dot product of this, and another vector */
	inline T dot(const Vector<T, N>& other) const {
		T dotProduct = 0;
		for (unsigned int i = 0; i < N; i++)
			dotProduct += values[i] * other[i];
		return dotProduct;
	}

	/* Returns a string representation of the vector */
	std::string toString() {
		std::string value = "(";
		for (unsigned int i = 0; i < N; i++) {
			value += utils_string::str(values[i]);
			if (i != N - 1)
				value += ",";
		}
		return value + ")";
	}

	/* Returns a pointer to the data array */
	inline T* data() { return values; }

	/* Returns the number of elements stored by this vector */
	inline unsigned int getNumElements() { return N; }
};

/*****************************************************************************
 * Various implementations of Vector
 *****************************************************************************/

template <typename T>
class Vector2 : public Vector<T, 2> {
public:
	Vector2() {}
	Vector2(const Vector<T, 2> &base) : Vector<T, 2>(base) {}
	Vector2(T value) { (*this)[0] = value; (*this)[1] = value; }
	Vector2(T x, T y) { (*this)[0] = x; (*this)[1] = y; }
	Vector2(const Vector<T, 3> &base) { setX(base[0]); setY(base[1]); }

	inline void setX(T x) { (*this)[0] = x; }
	inline void setY(T y) { (*this)[1] = y; }
	inline T getX() const { return (*this)[0]; }
	inline T getY() const { return (*this)[1]; }
};

template <typename T>
class Vector3 : public Vector<T, 3> {
public:
	Vector3() {}
	Vector3(const Vector<T, 3> &base) : Vector<T, 3>(base) {}
	Vector3(T value) { (*this)[0] = value; (*this)[1] = value; (*this)[2] = value; }
	Vector3(T x, T y, T z) { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; }
	Vector3(const Vector<T, 2> &base, T z) { setX(base[0]); setY(base[1]); setZ(z); }
	Vector3(const Vector<T, 4> &base) { setX(base[0]); setY(base[1]); setZ(base[2]); }

	inline Vector3<T> cross(Vector3<T> other) {
	    return Vector3<T>(getY() * other.getZ() - getZ() * other.getY(),
	                	  getZ() * other.getX() - getX() * other.getZ(),
						  getX() * other.getY() - getY() * other.getX());
	}

	inline void setX(T x) { (*this)[0] = x; }
	inline void setY(T y) { (*this)[1] = y; }
	inline void setZ(T z) { (*this)[2] = z; }
	inline T getX() const { return (*this)[0]; }
	inline T getY() const { return (*this)[1]; }
	inline T getZ() const { return (*this)[2]; }
};

template <typename T>
class Vector4 : public Vector<T, 4> {
public:
	Vector4() {}
	Vector4(const Vector<T, 4> &base) : Vector<T, 4>(base) {}
	Vector4(T value) { (*this)[0] = value; (*this)[1] = value; (*this)[2] = value; (*this)[3] = value; }
	Vector4(T x, T y, T z, T w) { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; (*this)[3] = w; }
	Vector4(const Vector<T, 3> &base, T w) { setX(base[0]); setY(base[1]); setZ(base[2]); setW(w); }

	inline void setX(T x) { (*this)[0] = x; }
	inline void setY(T y) { (*this)[1] = y; }
	inline void setZ(T z) { (*this)[2] = z; }
	inline void setW(T w) { (*this)[3] = w; }
	inline T getX() const { return (*this)[0]; }
	inline T getY() const { return (*this)[1]; }
	inline T getZ() const { return (*this)[2]; }
	inline T getW() const { return (*this)[3]; }
};

/*****************************************************************************
 * The specific versions of a Vector
 *****************************************************************************/

class Vector2i : public Vector2<int> {
public:
	Vector2i(const Vector<int, 2> &base) : Vector2<int>(base) {}
	Vector2i(const Vector2<int> &base) : Vector2<int>(base) {}
	Vector2i(int value = 0) : Vector2<int>(value, value) {}
	Vector2i(int x, int y) : Vector2<int>(x, y) {}
};

class Vector2f : public Vector2<float> {
public:
	Vector2f(const Vector<float, 2> &base) : Vector2<float>(base) {}
	Vector2f(const Vector2<float> &base) : Vector2<float>(base) {}
	Vector2f(float value = 0) : Vector2<float>(value) {}
	Vector2f(float x, float y) : Vector2<float>(x, y) {}
};

class Vector2d : public Vector2<double> {
public:
	Vector2d(const Vector<double, 2> &base) : Vector2<double>(base) {}
	Vector2d(const Vector2<double> &base) : Vector2<double>(base) {}
	Vector2d(double value = 0) : Vector2<double>(value) {}
	Vector2d(double x, double y) : Vector2<double>(x, y) {}
};

class Vector3i : public Vector3<int> {
public:
	Vector3i(const Vector<int, 3> &base) : Vector3<int>(base) {}
	Vector3i(const Vector3<int> &base) : Vector3<int>(base) {}
	Vector3i(int value = 0) : Vector3<int>(value) {}
	Vector3i(int x, int y, int z) : Vector3<int>(x, y, z) {}
	Vector3i(const Vector<int, 2> &base, int z) : Vector3<int>(base, z) {}
	Vector3i(const Vector2<int> &base, int z) : Vector3<int>(base, z) {}
};

class Vector3f : public Vector3<float> {
public:
	Vector3f(const Vector<float, 3> &base) : Vector3<float>(base) {}
	Vector3f(const Vector3<float> &base) : Vector3<float>(base) {}
	Vector3f(float value = 0) : Vector3<float>(value) {}
	Vector3f(float x, float y, float z) : Vector3<float>(x, y, z) {}
	Vector3f(Vector2f vec2, float z = 0) : Vector3<float>(vec2.getX(), vec2.getY(), 0) {}
	Vector3f(const Vector<float, 2> &base, float z) : Vector3<float>(base, z) {}
	Vector3f(const Vector2<float> &base, float z) : Vector3<float>(base, z) {}
};

class Vector3d : public Vector3<double> {
public:
	Vector3d(const Vector<double, 3> &base) : Vector3<double>(base) {}
	Vector3d(const Vector3<double> &base) : Vector3<double>(base) {}
	Vector3d(double value = 0) : Vector3<double>(value) {}
	Vector3d(double x, double y, double z) : Vector3<double>(x, y, z) {}
	Vector3d(const Vector<double, 2> &base, double z) : Vector3<double>(base, z) {}
	Vector3d(const Vector2<double> &base, double z) : Vector3<double>(base, z) {}
};

class Vector4i : public Vector4<int> {
public:
	Vector4i(const Vector<int, 4> &base) : Vector4<int>(base) {}
	Vector4i(const Vector4<int> &base) : Vector4<int>(base) {}
	Vector4i(int value = 0) : Vector4<int>(value) {}
	Vector4i(int x, int y, int z, int w) : Vector4<int>(x, y, z, w) {}
	Vector4i(const Vector<int, 3> &base, int w) : Vector4<int>(base, w) {}
	Vector4i(const Vector3<int> &base, int w) : Vector4<int>(base, w) {}
};

class Vector4f : public Vector4<float> {
public:
	Vector4f(const Vector<float, 4> &base) : Vector4<float>(base) {}
	Vector4f(const Vector4<float> &base) : Vector4<float>(base) {}
	Vector4f(float value = 0) : Vector4<float>(value) {}
	Vector4f(float x, float y, float z, float w) : Vector4<float>(x, y, z, w) {}
	Vector4f(const Vector<float, 3> &base, float w) : Vector4<float>(base, w) {}
	Vector4f(const Vector3<float> &base, float w) : Vector4<float>(base, w) {}
};

class Vector4d : public Vector4<double> {
public:
	Vector4d(const Vector<double, 4> &base) : Vector4<double>(base) {}
	Vector4d(const Vector4<double> &base) : Vector4<double>(base) {}
	Vector4d(double value = 0) : Vector4<double>(value) {}
	Vector4d(double x, double y, double z, double w) : Vector4<double>(x, y, z, w) {}
	Vector4d(const Vector<double, 3> &base, double w) : Vector4<double>(base, w) {}
	Vector4d(const Vector3<double> &base, double w) : Vector4<double>(base, w) {}
};


