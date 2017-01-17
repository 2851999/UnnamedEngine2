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
#ifndef CORE_MATRIX_H_
#define CORE_MATRIX_H_

#include "Quaternion.h"
#include "Vector.h"

/*****************************************************************************
 * The generalised version of a Matrix
 *****************************************************************************/

/* Assumes a N by N size */
template<typename T, unsigned int N>
class Matrix {
private:
	T values[N][N];
public:
	Matrix() {
		//Assign all values to 0
		for (unsigned int col = 0; col < N; col++) {
			for (unsigned int row = 0; row < N; row++)
				values[col][row] = 0;
		}
	}

	/* Various operations */
	inline void set(unsigned int row, unsigned int col, T value) { values[col][row] = value; } //COLUMN MAJOR ORDER!!!!!
	inline T get(unsigned int row, unsigned int col)       const { return values[col][row]; }

	/* Returns the result of adding another matrix to this one */
	inline Matrix<T, N> operator+(const Matrix<T, N>& other) const {
		Matrix<T, N> result;
		//Go through each value and assign the result to the sum of the two
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				result.set(row, col, get(row, col) + other.get(row, col));
		}
		return result;
	}

	/* Returns the result of subtracting another matrix from this one */
	inline Matrix<T, N> operator-(const Matrix<T, N>& other) const {
		Matrix<T, N> result;
		//Go through each value and assign the result to the subtraction of the two
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				result.set(row, col, get(row, col) - other.get(row, col));
		}
		return result;
	}

	/* Returns the result of multiplying this matrix by a scalar */
	inline Matrix<T, N> operator*(const T& value) const {
		Matrix<T, N> result;
		//Go through each value and multiply it by the scalar value
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				result.set(row, col, get(row, col) * value);
		}
		return result;
	}

	/* Returns the result of 'dividing' this matrix by a scalar */
	inline Matrix<T, N> operator/(const T& value) const {
		Matrix<T, N> result;
		//Go through each value and divide it by the scalar value
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				result.set(row, col, get(row, col) / value);
		}
		return result;
	}

	/* Adds another matrix to this one */
	inline Matrix<T, N>& operator+=(const Matrix<T, N>& other) {
		//Go through each value and set it to the sum of the two
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				set(row, col, get(row, col) + other.get(row, col));
		}
		return (*this);
	}

	/* Subtracts another matrix from this one */
	inline Matrix<T, N>& operator-=(const Matrix<T, N>& other) {
		//Go through each value and set it to the subtraction of the two
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				set(row, col, get(row, col) - other.get(row, col));
		}
		return (*this);
	}

	/* Multiplies this matrix by a scalar */
	inline Matrix<T, N>& operator*=(const T& value) {
		//Go through each value and multiply it by the scalar
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				set(row, col, get(row, col) * value);
		}
		return (*this);
	}

	/* Divides this matrix by a scalar */
	inline Matrix<T, N>& operator/=(const T& value) {
		//Go through each value and divide it by the scalar
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				set(row, col, get(row, col) / value);
		}
		return (*this);
	}

	/* Returns the result of multiplying this matrix by another */
	inline Matrix<T, N> operator*(const Matrix<T, N>& other) const {
		Matrix<T, N> result;
		//Go through each row and column
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++) {
				//Go through the required number of additions required
				for (unsigned int i = 0; i < N; i++)
					//Add onto the current value, to perform the matrix multiplication
					result.set(row, col, result.get(row, col) + (get(row, i) * other.get(i, col)));
			}
		}
		return result;
	}

	/* Returns the result of multiplying this matrix a vector */
	inline Vector<T, N> operator*(const Vector<T, N>& vector) const {
		Vector<T, N> result;
		for (unsigned int i = 0; i < N; i++) {
			result[i] = 0;
			for (unsigned int j = 0; j < N; j++) {
				result[i] += vector[j] * get(i, j);
			}
		}
		return result;
	}

	/* Multiplies this matrix by another */
	inline Matrix<T, N>& operator*=(const Matrix<T, N>& other) {
		//Calculate the result of multiplying this matrix by the other one
		Matrix<T, N> result = (*this) * other;
		//Assign all of the values within this matrix now that the data is no longer needed
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				set(row, col, result.get(row, col));
		}
		return (*this);
	}

	/* Comparison operators */
	inline bool operator==(Matrix<T, N> other) {
		//Go through all of the data, and check whether the values in this matrix
		//are the same as in the 'other' matrix
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				if (other.get(row, col) != get(row, col))
					return false;
		}
		return true;
	}

	/* Returns the transpose of this matrix */
	inline Matrix<T, N> transpose() {
		Matrix<T, N> result;
		//Go through and flip the rows and columns
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++)
				result.set(row, col, get(col, row));
		}
		return result;
	}

	/* Sets this matrix to its identity */
	inline void setIdentity() {
		//Go through all of the values in this matrix
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++) {
				//Assign the diagonal to 1, but all other values to 0
				if (row == col)
					set(row, col, 1);
				else
					set(row, col, 0);
			}
		}
	}

	/* Set's this matrix to its identity and then returns itself */
	inline Matrix<T, N>& initIdentity() {
		setIdentity();
		return (*this);
	}

	/* Returns a string representation of this matrix */
	std::string toString() {
		//The string to return
		std::string value = "";
		//Go through all of the values
		for (unsigned int row = 0; row < N; row++) {
			for (unsigned int col = 0; col < N; col++) {
				//Add the current value onto the string
				value += StrUtils::str(get(row, col));
				//Check whether a comma should be added, and if so, add one
				if (col != N - 1)
					value += ",";
			}
			//Start a new line if at the end of this row
			if (row != N - 1)
				value += "\n";
		}
		//Return the string
		return value;
	}

	/* Returns a pointer to the front of the values stored in this matrix */
	inline const T* front() { return &values[0][0]; }

	/* Returns a pointer to the data array */
	inline T* data() { return values[0]; }

	/* Returns the number of values stored in this matrix */
	inline int getNumElements() { return N * N; }
	/* Returns the size of this matrix (in bytes) */
	inline int getSize() { return N * N * sizeof(values[0][0]); }
};

/*****************************************************************************
 * Various implementations of Matrix
 *****************************************************************************/

template<typename T>
class Matrix2 : public Matrix<T, 2> {
public:
	Matrix2() : Matrix<T, 2>() {}
	Matrix2(const Matrix<T, 2> &base) : Matrix<T, 2>(base) {}
};

template<typename T>
class Matrix3 : public Matrix<T, 3> {
public:
	Matrix3() : Matrix<T, 3>() {}
	Matrix3(const Matrix<T, 3> &base) : Matrix<T, 3>(base) {}
};

template<typename T>
class Matrix4 : public Matrix<T, 4> {
public:
	Matrix4() : Matrix<T, 4>() {}
	Matrix4(const Matrix<T, 4> &base) : Matrix<T, 4>(base) {}
};

/*****************************************************************************
 * The specific versions of a Matrix
 *****************************************************************************/

class Matrix2i : public Matrix2<int> {
public:
	Matrix2i() : Matrix2<int>() {}
	Matrix2i(const Matrix<int, 2> &base) : Matrix2<int>(base) {}
	Matrix2i(const Matrix2<int> &base) : Matrix2<int>(base) {}
};

class Matrix2f : public Matrix2<float> {
public:
	Matrix2f() : Matrix2<float>() {}
	Matrix2f(const Matrix<float, 2> &base) : Matrix2<float>(base) {}
	Matrix2f(const Matrix2<float> &base) : Matrix2<float>(base) {}
};

class Matrix2d : public Matrix2<double> {
public:
	Matrix2d() : Matrix2<double>() {}
	Matrix2d(const Matrix<double, 2> &base) : Matrix2<double>(base) {}
	Matrix2d(const Matrix2<double> &base) : Matrix2<double>(base) {}
};

class Matrix3i : public Matrix3<int> {
public:
	Matrix3i() : Matrix3<int>() {}
	Matrix3i(const Matrix<int, 3> &base) : Matrix3<int>(base) {}
	Matrix3i(const Matrix3<int> &base) : Matrix3<int>(base) {}
};

class Matrix3f : public Matrix3<float> {
public:
	Matrix3f() : Matrix3<float>() {}
	Matrix3f(const Matrix<float, 3> &base) : Matrix3<float>(base) {}
	Matrix3f(const Matrix3<float> &base) : Matrix3<float>(base) {}

	/* Returns the inverse of this 3x3 matrix */
	Matrix3f inverse() {
		Matrix3f result;

		//Assign each value letters to make the calculation easier to follow
		float a = get(0, 0); float b = get(0, 1); float c = get(0, 2);
		float d = get(1, 0); float e = get(1, 1); float f = get(1, 2);
		float g = get(2, 0); float h = get(2, 1); float i = get(2, 2);

		//Now calculate the matrix of cofactors, using capital letters in the
		//same order as the lower case ones
		float A = (e * i - f * h);
		float B = (f * g - d * i);
		float C = (d * h - e * g);
		float D = (h * c - i * b);
		float E = (i * a - g * c);
		float F = (g * b - h * a);
		float G = (b * f - c * e);
		float H = (c * d - a * f);
		float I = (a * e - b * d);

		//From this, the inverse of the determinant is calculated so it
		//can be multiplied by the transpose of the matrix of cofactors
		//to get the inverse
		float det = (a * A + b * B + c * C);
		//Prevent a divide by 0 error
		if (det != 0.0f) {
			float invDet = 1.0f / (a * A + b * B + c * C);

			//Finally assign the result
			result.set(0, 0, invDet * A); result.set(0, 1, invDet * D); result.set(0, 2, invDet * G);
			result.set(1, 0, invDet * B); result.set(1, 1, invDet * E); result.set(1, 2, invDet * H);
			result.set(2, 0, invDet * C); result.set(2, 1, invDet * F); result.set(2, 2, invDet * I);
		}

		return result;
	}
};

class Matrix3d : public Matrix3<double> {
public:
	Matrix3d() : Matrix3<double>() {}
	Matrix3d(const Matrix<double, 3> &base) : Matrix3<double>(base) {}
	Matrix3d(const Matrix3<double> &base) : Matrix3<double>(base) {}
};

class Matrix4i : public Matrix4<int> {
public:
	Matrix4i() : Matrix4<int>() {}
	Matrix4i(const Matrix<int, 4> &base) : Matrix4<int>(base) {}
	Matrix4i(const Matrix4<int> &base) : Matrix4<int>(base) {}
};

class Matrix4f : public Matrix4<float> {
public:
	Matrix4f() : Matrix4<float>() {}
	Matrix4f(const Matrix<float, 4> &base) : Matrix4<float>(base) {}
	Matrix4f(const Matrix4<float> &base) : Matrix4<float>(base) {}

	const Matrix4f& initFromVectors(const Vector3f& forward, const Vector3f& up, const Vector3f& right) {
		set(0, 0, right.getX()); set(0, 1, right.getY()); set(0, 2, right.getZ()); set(0, 3, 0);
		set(1, 0, up.getX()); set(1, 1, up.getY()); set(1, 2, up.getZ()); set(1, 3, 0);
		set(2, 0, forward.getX()); set(2, 1, forward.getY()); set(2, 2, forward.getZ()); set(2, 3, 0);
		set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);

		return (*this);
	}

	const Matrix4f& initOrthographic(float left, float right, float bottom, float top, float zNear, float zFar) {
		set(0, 0, 2.0f / (right - left));
		set(0, 1, 0);
		set(0, 2, 0);
		set(0, 3, -(right + left) / (right - left));

		set(1, 0, 0);
		set(1, 1, 2.0f / (top - bottom));
		set(1, 2, 0);
		set(1, 3, -(top + bottom) / (top - bottom));

		set(2, 0, 0);
		set(2, 1, 0);
		set(2, 2, -2.0f / (zFar - zNear));
		set(2, 3, -(zFar + zNear) / (zFar - zNear));

		set(3, 0, 0);
		set(3, 1, 0);
		set(3, 2, 0);
		set(3, 3, 1);

		return (*this);
	}

	const Matrix4f& initFrustum(float left, float right, float bottom, float top, float zNear, float zFar) {
		set(0, 0, 2.0f * zNear / (right - left));
		set(0, 1, 0);
		set(0, 2, 0);
		set(0, 3, 0);

		set(1, 0, 0);
		set(1, 1, 2.0f * zNear / (top - bottom));
		set(1, 2, 0);
		set(1, 3, 0);

		set(2, 0, (right + left) / (right - left));
		set(2, 1, (top + bottom) / (top - bottom));
		set(2, 2, -(zFar + zNear) / (zFar - zNear));
		set(2, 3, -1);

		set(3, 0, 0);
		set(3, 1, 0);
		set(3, 2, -2.0f * zFar * zNear / (zFar - zNear));
		set(3, 3, 1);

		return (*this);
	}

	const Matrix4f& initPerspective(float fovy, float aspect, float zNear, float zFar) {
		float scale = (tan(fovy / 2 * (MathsUtils::PI / 360)));

		set(0, 0, 1.0f / (aspect * scale)); set(0, 1, 0); set(0, 2, 0); set(0, 3, 0);
		set(1, 0, 0); set(1, 1, 1.0f / scale); set(1, 2, 0); set(1, 3, 0);
		set(2, 0, 0); set(2, 1, 0); set(2, 2, -(zFar + zNear) / (zFar - zNear)); set(2, 3, -(2 * zFar * zNear) / (zFar - zNear));
		set(3, 0, 0); set(3, 1, 0); set(3, 2, -1); set(3, 3, 1);

		return (*this);
	}

	const Matrix4f& initTranslation(const Vector2f& vector) {
		set(0, 0, 1); set(0, 1, 0); set(0, 2, 0); set(0, 3, vector.getX());
		set(1, 0, 0); set(1, 1, 1); set(1, 2, 0); set(1, 3, vector.getY());
		set(2, 0, 0); set(2, 1, 0); set(2, 2, 1); set(2, 3, 0);
		set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);

		return (*this);
	}

	const Matrix4f& initTranslation(const Vector3f& vector) {
		set(0, 0, 1); set(0, 1, 0); set(0, 2, 0); set(0, 3, vector.getX());
		set(1, 0, 0); set(1, 1, 1); set(1, 2, 0); set(1, 3, vector.getY());
		set(2, 0, 0); set(2, 1, 0); set(2, 2, 1); set(2, 3, vector.getZ());
		set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);

		return (*this);
	}

	const Matrix4& initRotation(float angle, bool x, bool y, bool z) {
		float c = (float) cos(MathsUtils::toRadians(angle));
		float s = (float) sin(MathsUtils::toRadians(angle));

		if (x) {
			set(0, 0, 1); set(0, 1, 0); set(0, 2, 0); set(0, 3, 0);
			set(1, 0, 0); set(1, 1, c); set(1, 2, -s); set(1, 3, 0);
			set(2, 0, 0); set(2, 1, s); set(2, 2, c); set(2, 3, 0);
			set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);
		} else if (y) {
			set(0, 0, c); set(0, 1, 0); set(0, 2, s); set(0, 3, 0);
			set(1, 0, 0); set(1, 1, 1); set(1, 2, 0); set(1, 3, 0);
			set(2, 0, -s); set(2, 1, 0); set(2, 2, c); set(2, 3, 0);
			set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);
		} else if (z) {
			set(0, 0, c); set(0, 1, -s); set(0, 2, 0); set(0, 3, 0);
			set(1, 0, s); set(1, 1, c); set(1, 2, 0); set(1, 3, 0);
			set(2, 0, 0); set(2, 1, 0); set(2, 2, 1); set(2, 3, 0);
			set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);
		}

		return (*this);
	}

	const Matrix4f& initScale(const Vector2f& vector) {
		set(0, 0, vector.getX()); set(0, 1, 0); set(0, 2, 0); set(0, 3, 0);
		set(1, 0, 0); set(1, 1, vector.getY()); set(1, 2, 0); set(1, 3, 0);
		set(2, 0, 0); set(2, 1, 0); set(2, 2, 1); set(2, 3, 0);
		set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);

		return (*this);
	}

	const Matrix4f& initScale(const Vector3f& vector) {
		set(0, 0, vector.getX()); set(0, 1, 0); set(0, 2, 0); set(0, 3, 0);
		set(1, 0, 0); set(1, 1, vector.getY()); set(1, 2, 0); set(1, 3, 0);
		set(2, 0, 0); set(2, 1, 0); set(2, 2, vector.getZ()); set(2, 3, 0);
		set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);

		return (*this);
	}

	const Matrix4f& initLookAt(const Vector3f& eye, const Vector3f& centre, const Vector3f& up) {
		//EXPLANATION: http://stackoverflow.com/questions/21152556/an-inconsistency-in-my-understanding-of-the-glm-lookat-function

		Vector3f forward = (centre - eye).normalise();
		Vector3f u = up.normalised();
		Vector3f side = forward.cross(u).normalise();
		u = side.cross(forward);

		set(0, 0, side.getX());     set(0, 1, side.getY());     set(0, 2, side.getZ());     set(0, 3, -side.dot(eye));
		set(1, 0, u.getX());        set(1, 1, u.getY());        set(1, 2, u.getZ());        set(1, 3, -up.dot(eye));
		set(2, 0, -forward.getX()); set(2, 1, -forward.getY()); set(2, 2, -forward.getZ()); set(2, 3, forward.dot(eye));
		set(3, 0, 0);               set(3, 1, 0);               set(3, 2, 0);               set(3, 3, 1);

		return (*this);
	}

	inline void translate(Vector2f vector) { (*this) *= Matrix4f().initTranslation(vector); }
	inline void translate(Vector3f vector) { (*this) *= Matrix4f().initTranslation(vector); }

	inline void rotate(Quaternion angle) { (*this) *= angle.toRotationMatrix(); }
	inline void rotate(float angle, bool x, bool y, bool z) { (*this) *= Matrix4f().initRotation(angle, x, y, z); }
	inline void rotate(float angle) { rotate(angle, 0, 0, 1); }
	inline void rotate(Vector2f angles){
		rotate(angles.getX(), 1, 0, 0);
		rotate(angles.getY(), 0, 1, 0);
	}
	inline void rotate(Vector3f angles) {
		rotate(angles.getX(), 1, 0, 0);
		rotate(angles.getY(), 0, 1, 0);
		rotate(angles.getZ(), 0, 0, 1);
	}

	inline void scale(Vector2f vector) { (*this) *= Matrix4f().initScale(vector); }
	inline void scale(Vector3f vector) { (*this) *= Matrix4f().initScale(vector); }

	inline void transform(Vector2f t, float r, Vector2f s) {
		translate(t);
		rotate(r);
		scale(s);
	}

	inline void transform(Vector3f t, Vector3f r, Vector3f s) {
		translate(t);
		rotate(r);
		scale(s);
	}

	inline void transform(Vector3f t, Quaternion r, Vector3f s) {
		translate(t);
		rotate(r);
		scale(s);
	}

	inline void transformR(Vector2f t, float r, Vector2f s) {
		scale(s);
		rotate(r);
		translate(t);
	}

	inline void transformR(Vector3f t, Quaternion r, Vector3f s) {
		scale(s);
		rotate(r);
		translate(t);
	}

	inline Matrix3f to3x3() {
		Matrix3f matrix;

		matrix.set(0, 0, get(0, 0)); matrix.set(0, 1, get(0, 1)); matrix.set(0, 2, get(0, 2));
		matrix.set(1, 0, get(1, 0)); matrix.set(1, 1, get(1, 1)); matrix.set(1, 2, get(1, 2));
		matrix.set(2, 0, get(2, 0)); matrix.set(2, 1, get(2, 1)); matrix.set(2, 2, get(2, 2));

		return matrix;
	}

//	set(0, 0, 1); set(0, 1, 0); set(0, 2, 0); set(0, 3, 0);
//	set(1, 0, 0); set(1, 1, 1); set(1, 2, 0); set(1, 3, 0);
//	set(2, 0, 0); set(2, 1, 0); set(2, 2, 1); set(2, 3, 0);
//	set(3, 0, 0); set(3, 1, 0); set(3, 2, 0); set(3, 3, 1);
};

class Matrix4d : public Matrix4<double> {
public:
	Matrix4d() : Matrix4<double>() {}
	Matrix4d(const Matrix<double, 4> &base) : Matrix4<double>(base) {}
	Matrix4d(const Matrix4<double> &base) : Matrix4<double>(base) {}
};

#endif /* CORE_MATRIX_H_ */
