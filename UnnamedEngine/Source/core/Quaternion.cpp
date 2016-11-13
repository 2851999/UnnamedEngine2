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
