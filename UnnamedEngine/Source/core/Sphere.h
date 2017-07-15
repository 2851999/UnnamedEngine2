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

#ifndef CORE_SPHERE_H_
#define CORE_SPHERE_H_

/*****************************************************************************
 * The Sphere class defines a sphere
 *****************************************************************************/

class Sphere {
public:
	/* The centre of this sphere */
	Vector3f centre;

	/* The radius of this sphere */
	float radius = 0.0f;

	/* The constructors */
	Sphere() {}
	Sphere(Vector3f centre, float radius) : centre(centre), radius(radius) {}
	Sphere(float x, float y, float z, float radius) : centre(Vector3f(x, y, z)), radius(radius) {}

	/* The destructor */
	virtual ~Sphere() {}

	/* Method to check whether this sphere contains a point */
	inline bool contains(Vector3f& point) {
		return (centre - point).length() < radius;
	}

	/* Method to check whether this sphere intersects with a point */
	inline bool intersects(Vector3f& point) {
		return (centre - point).length() <= radius;
	}

	/* Method to check whether this sphere intersects with another sphere */
	inline bool intersects(Sphere& other) {
		return (centre - other.centre).length() <= (radius + other.radius);
	}
};

#endif /* CORE_SPHERE_H_ */
