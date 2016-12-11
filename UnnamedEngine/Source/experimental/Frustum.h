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

#ifndef CORE_RENDER_FRUSTUM_H_
#define CORE_RENDER_FRUSTUM_H_

#include "../core/Vector.h"
#include "../core/Matrix.h"

/*****************************************************************************
 * The FrustumPlane structure helps represent a plane that forms part of a
 * frustum
 *****************************************************************************/

struct FrustumPlane {
	//Coefficients for the plane's equation (ax + bx + cx + d = 0)
	float a, b, c, d;
};

/*****************************************************************************
 * The FrustumPlane class helps represent a view frustum
 *****************************************************************************/

class Frustum {
private:
	/* The 6 planes forming this frustum */
	FrustumPlane planes[6];

	/* Method called to calculate and return a plane */
	FrustumPlane calculatePlane(float a, float b, float c, float z);
public:
	/* The constructor */
	Frustum() {}

	/* The destructor */
	virtual ~Frustum() {}

	/* Method called to update this frustum given the projection view matrix */
	void update(Matrix4f pvm);

	/* Method called to check whether a sphere is within this frustum */
	bool sphereInFrustum(Vector3f centre, float radius);

	/* Method called to check whether a point is within this frustum */
	bool pointInFrustum(Vector3f point);
};

#endif /* CORE_RENDER_FRUSTUM_H_ */
