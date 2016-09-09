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

#ifndef CORE_VECTOR_H_
#define CORE_VECTOR_H_

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

	inline Vector<T, N> operator+(Vector<T, N> other) {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] + other[i];
		return result;
	}

	inline Vector<T, N> operator-(Vector<T, N> other) {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] - other[i];
		return result;
	}

	inline Vector<T, N> operator*(Vector<T, N> other) {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] * other[i];
		return result;
	}

	inline Vector<T, N> operator/(Vector<T, N> other) {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] / other[i];
		return result;
	}

	inline void operator+=(Vector<T, N> other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] += other[i];
	}

	inline void operator-=(Vector<T, N> other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] -= other[i];
	}

	inline void operator*=(Vector<T, N> other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] *= other[i];
	}

	inline void operator/=(Vector<T, N> other) {
		for (unsigned int i = 0; i < N; i++)
			values[i] /= other[i];
	}

	inline Vector<T, N> operator*(T value) {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] * value;
		return result;
	}

	inline Vector<T, N> operator/(T value) {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++)
			result[i] = values[i] / value;
		return result;
	}

	/* Various comparison operators */
	inline void operator*=(T value) {
		for (unsigned int i = 0; i < N; i++)
			values[i] *= value;
	}

	inline void operator/=(T value) {
		for (unsigned int i = 0; i < N; i++)
			values /= value;
	}

	inline bool operator==(Vector<T, N> other) {
		for (unsigned int i = 0; i < N; i++) {
			if (other[i] != values[i])
				return false;
		}
		return true;
	}

	inline bool operator!=(Vector<T, N> other) { return ! ((*this) == other);       }
	inline bool operator<(Vector<T, N> other)  { return length() < other.length();  }
	inline bool operator<=(Vector<T, N> other) { return length() <= other.length(); }
	inline bool operator>(Vector<T, N> other)  { return length() > other.length();  }
	inline bool operator>=(Vector<T, N> other) { return length() >= other.length(); }

	/* Finds the total length of the vector */
	inline T length() {
		double total = 0;
		for (unsigned int i = 0; i < N; i++)
			total += (values[i] * values[i]);
		return sqrt(total);
	}

	/* Returns the unit vector */
	inline Vector<T, N> normalise() {
		T length = this->length();
		if (length != 0) {
			for (unsigned int i = 0; i < N; i++)
				values[i] /= length;
		}
		return (*this);
	}

	/* Returns the dot product of this, and another vector */
	inline T dot(Vector<T, N> other) {
		T dotProduct = 0;
		for (unsigned int i = 0; i < N; i++)
			dotProduct += values[i] * other[i];
		return dotProduct;
	}

	/* Returns a string representation of the vector */
	std::string toString() {
		std::string value = "(";
		for (unsigned int i = 0; i < N; i++) {
			value += StrUtils::str(values[i]);
			if (i != N - 1)
				value += ",";
		}
		return value + ")";
	}
};

/*****************************************************************************
 * Various implementations of Vector
 *****************************************************************************/

template <typename T>
class Vector2 : public Vector<T, 2> {
public:
	Vector2() {}
	Vector2(const Vector<T, 2> &base) : Vector<T, 2>(base) {}
	Vector2(T x, T y) { (*this)[0] = x; (*this)[1] = y; }

	inline void setX(T x) { (*this)[0] = x; }
	inline void setY(T y) { (*this)[1] = y; }
	inline T getX() { return (*this)[0]; }
	inline T getY() { return (*this)[1]; }
};

template <typename T>
class Vector3 : public Vector<T, 3> {
public:
	Vector3() {}
	Vector3(const Vector<T, 3> &base) : Vector<T, 3>(base) {}
	Vector3(T x, T y, T z) { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; }

	inline Vector3<T> cross(Vector3<T> other) {
	    return Vector3<T>(getY() * other.getZ() - getZ() * other.getY(),
	                	  getZ() * other.getX() - getX() * other.getZ(),
						  getX() * other.getY() - getY() * other.getX());
	}

	inline void setX(T x) { (*this)[0] = x; }
	inline void setY(T y) { (*this)[1] = y; }
	inline void setZ(T z) { (*this)[2] = z; }
	inline T getX() { return (*this)[0]; }
	inline T getY() { return (*this)[1]; }
	inline T getZ() { return (*this)[2]; }
};

template <typename T>
class Vector4 : public Vector<T, 4> {
public:
	Vector4() {}
	Vector4(const Vector<T, 4> &base) : Vector<T, 4>(base) {}
	Vector4(T x, T y, T z, T w) { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; (*this)[3] = w; }

	inline void setX(T x) { (*this)[0] = x; }
	inline void setY(T y) { (*this)[1] = y; }
	inline void setZ(T z) { (*this)[2] = z; }
	inline void setW(T w) { (*this)[3] = w; }
	inline T getX() { return (*this)[0]; }
	inline T getY() { return (*this)[1]; }
	inline T getZ() { return (*this)[2]; }
	inline T getW() { return (*this)[3]; }
};

/*****************************************************************************
 * The specific versions of a Vector
 *****************************************************************************/

class Vector2i : public Vector2<int> {
public:
	Vector2i(const Vector<int, 2> &base) : Vector2<int>(base) {}
	Vector2i(const Vector2<int> &base) : Vector2<int>(base) {}
	Vector2i(int x = 0, int y = 0) : Vector2<int>(x, y) {}
};

class Vector2f : public Vector2<float> {
public:
	Vector2f(const Vector<float, 2> &base) : Vector2<float>(base) {}
	Vector2f(const Vector2<float> &base) : Vector2<float>(base) {}
	Vector2f(float x = 0, float y = 0) : Vector2<float>(x, y) {}
};

class Vector2d : public Vector2<double> {
public:
	Vector2d(const Vector<double, 2> &base) : Vector2<double>(base) {}
	Vector2d(const Vector2<double> &base) : Vector2<double>(base) {}
	Vector2d(double x = 0, double y = 0) : Vector2<double>(x, y) {}
};

class Vector3i : public Vector3<int> {
public:
	Vector3i(const Vector<int, 3> &base) : Vector3<int>(base) {}
	Vector3i(const Vector3<int> &base) : Vector3<int>(base) {}
	Vector3i(int x = 0, int y = 0, int z = 0) : Vector3<int>(x, y, z) {}
};

class Vector3f : public Vector3<float> {
public:
	Vector3f(const Vector<float, 3> &base) : Vector3<float>(base) {}
	Vector3f(const Vector3<float> &base) : Vector3<float>(base) {}
	Vector3f(float x = 0, float y = 0, float z = 0) : Vector3<float>(x, y, z) {}
	Vector3f(Vector2f vec2, float z = 0) : Vector3<float>(vec2.getX(), vec2.getY(), 0) {}
};

class Vector3d : public Vector3<double> {
public:
	Vector3d(const Vector<double, 3> &base) : Vector3<double>(base) {}
	Vector3d(const Vector3<double> &base) : Vector3<double>(base) {}
	Vector3d(double x = 0, double y = 0, double z = 0) : Vector3<double>(x, y, z) {}
};

class Vector4i : public Vector4<int> {
public:
	Vector4i(const Vector<int, 4> &base) : Vector4<int>(base) {}
	Vector4i(const Vector4<int> &base) : Vector4<int>(base) {}
	Vector4i(int x = 0, int y = 0, int z = 0, int w = 0) : Vector4<int>(x, y, z, w) {}
};

class Vector4f : public Vector4<float> {
public:
	Vector4f(const Vector<float, 4> &base) : Vector4<float>(base) {}
	Vector4f(const Vector4<float> &base) : Vector4<float>(base) {}
	Vector4f(float x = 0, float y = 0, float z = 0, float w = 0) : Vector4<float>(x, y, z, w) {}
};

class Vector4d : public Vector4<double> {
public:
	Vector4d(const Vector<double, 4> &base) : Vector4<double>(base) {}
	Vector4d(const Vector4<double> &base) : Vector4<double>(base) {}
	Vector4d(double x = 0, double y = 0, double z = 0, double w = 0) : Vector4<double>(x, y, z,w) {}
};


#endif /* CORE_VECTOR_H_ */
