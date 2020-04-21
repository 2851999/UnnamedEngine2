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

#include "../core/Frustum.h"

/*****************************************************************************
 * The Frustum class
 *****************************************************************************/

FrustumPlane Frustum::calculatePlane(float a, float b, float c, float d) {
	float t = sqrt(a * a + b * b + c * c);
	FrustumPlane plane;
	plane.a = a / t;
	plane.b = b / t;
	plane.c = c / t;
	plane.d = d / t;
	return plane;
}

void Frustum::update(Matrix4f pvm) {
	//Right
	planes[0] = calculatePlane(pvm.get(3, 0) - pvm.get(0, 0),
							   pvm.get(3, 1) - pvm.get(0, 1),
							   pvm.get(3, 2) - pvm.get(0, 2),
							   pvm.get(3, 3) - pvm.get(0, 3));

	//Left
	planes[1] = calculatePlane(pvm.get(3, 0) + pvm.get(0, 0),
							   pvm.get(3, 1) + pvm.get(0, 1),
							   pvm.get(3, 2) + pvm.get(0, 2),
							   pvm.get(3, 3) + pvm.get(0, 3));

	//Top
	planes[2] = calculatePlane(pvm.get(3, 0) - pvm.get(1, 0),
							   pvm.get(3, 1) - pvm.get(1, 1),
							   pvm.get(3, 2) - pvm.get(1, 2),
							   pvm.get(3, 3) - pvm.get(1, 3));

	//Bottom
	planes[3] = calculatePlane(pvm.get(3, 0) + pvm.get(1, 0),
							   pvm.get(3, 1) + pvm.get(1, 1),
							   pvm.get(3, 2) + pvm.get(1, 2),
							   pvm.get(3, 3) + pvm.get(1, 3));

	//Far
	planes[4] = calculatePlane(pvm.get(3, 0) - pvm.get(2, 0),
							   pvm.get(3, 1) - pvm.get(2, 1),
							   pvm.get(3, 2) - pvm.get(2, 2),
							   pvm.get(3, 3) - pvm.get(2, 3));

	//Near
	planes[5] = calculatePlane(pvm.get(3, 0) + pvm.get(2, 0),
							   pvm.get(3, 1) + pvm.get(2, 1),
							   pvm.get(3, 2) + pvm.get(2, 2),
							   pvm.get(3, 3) + pvm.get(2, 3));
}

bool Frustum::sphereInFrustum(Vector3f centre, float radius) {
	for (int i = 0; i < 6; i++) {
		if (planes[i].a * centre.getX() + planes[i].b * centre.getY() + planes[i].c * centre.getZ() + planes[i].d <= -radius)
			return false;
	}
	return true;
}

bool Frustum::pointInFrustum(Vector3f point) {
	for (int i = 0; i < 6; i++) {
		if (planes[i].a * point.getX() + planes[i].b * point.getY() + planes[i].c * point.getZ() + planes[i].d < 0)
			return false;
	}
	return true;
}

//http://www.txutxi.com/?p=584
bool Frustum::AABBInFrustum(Vector3f min, Vector3f max) {
	Vector3f box[] = { min, max };
	for (unsigned int i = 0; i < 6; i++) {
		const FrustumPlane &p = planes[i];

		const int px = static_cast<int>(p.a > 0.0f);
		const int py = static_cast<int>(p.b > 0.0f);
		const int pz = static_cast<int>(p.c > 0.0f);

		const float dp = (p.a * box[px].getX()) + (p.b * box[py].getY()) + (p.c * box[pz].getZ());

		if (dp < -p.d) {
			return false;
		}
	}
	return true;
}
