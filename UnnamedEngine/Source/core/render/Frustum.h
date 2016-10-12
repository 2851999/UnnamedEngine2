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

#include "../Vector.h"

/*****************************************************************************
 * The FrustumPlane class helps represent the 6 planes of a Frustum
 *****************************************************************************/

class FrustumPlane {
private:
	Vector3f p;
	Vector3f n;
	float d = 0;
public:
	FrustumPlane() {}

	void init(Vector3f p0, Vector3f p1, Vector3f p2) {
		Vector3f v = p1 - p0;
		Vector3f u = p2 - p0;
		n = v.cross(u).normalise();
		p = p0;
		d = -n.dot(p);
	}

	void init(Vector3f normal, Vector3f point) {
		n = normal.normalise();
		d = -n.dot(point);
		p = point;
	}

	float distance(Vector3f other) {
		return d + n.dot(other);
	}
};

/*****************************************************************************
 * The FrustumPlane class helps represent a view frustum
 *****************************************************************************/

class Frustum {
private:
	FrustumPlane planes[6];

	Vector3f ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float zNear = 0, zFar = 0, aspect = 0, fov = 0, tang = 0;
	float nw = 0, nh = 0, fw = 0, fh = 0;

public:
	Frustum() {}

	void setProjection(float fov, float aspect, float zNear, float zFar) {
		this->fov = fov;
		this->aspect = aspect;
		this->zNear = zNear;
		this->zFar = zFar;

		tang = (float) tan(MathsUtils::toRadians(fov) * 0.5) ;
		nh = zNear * tang;
		nw = nh * aspect;
		fh = zFar  * tang;
		fw = fh * aspect;
	}

	void setView(Vector3f eye, Vector3f centre, Vector3f up) {
		Vector3f dir, nc, fc, X, Y, Z;

		Z = eye - centre;
		Z.normalise();

		X = up.cross(Z);
		X.normalise();

		Y = Z.cross(X);

		nc = eye - Z * zNear;
		fc = eye - Z * zFar;

		ntl = nc + Y * nh - X * nw;
		ntr = nc + Y * nh + X * nw;
		nbl = nc - Y * nh - X * nw;
		nbr = nc - Y * nh + X * nw;

		ftl = fc + Y * fh - X * fw;
		ftr = fc + Y * fh + X * fw;
		fbl = fc - Y * fh - X * fw;
		fbr = fc - Y * fh + X * fw;

		planes[0].init(ntr, ntl, ftl);
		planes[1].init(nbl, nbr, fbr);
		planes[2].init(ntl, nbl, fbl);
		planes[3].init(nbr, ntr, fbr);
		planes[4].init(ntl, ntr, nbr);
		planes[5].init(ftr, ftl, fbl);
	}

	bool testSphere(Vector3f position, float radius) {
		for (int i = 0; i < 6; i++) {
			float distance = planes[i].distance(position);
			if (distance < -radius)
				return false;
		}
		return true;
	}
};

#endif /* CORE_RENDER_FRUSTUM_H_ */
