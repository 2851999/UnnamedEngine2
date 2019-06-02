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

#ifndef CORE_RECTANGLE_H_
#define CORE_RECTANGLE_H_

#include "Vector.h"

/*****************************************************************************
 * The Rect class defines a rectangle
 *****************************************************************************/

class Rect {
public:
	/* The position and size */
	float x;
	float y;
	float width;
	float height;

	/* The constructors */
	Rect(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}
	Rect(Vector2f position, Vector2f size) : x(position.getX()), y(position.getY()), width(size.getX()), height(size.getY()) {}

	/* Returns whether a point lies within the boundary of this rectangle */
	inline bool contains(float x, float y) {
		return (x > this->x && y > this->y && x < this->x + this->width && y < this->y + this->height);
	}

	/* Returns whether a point lies within/on the boundary of this rectangle */
	inline bool intersects(float x, float y) {
		return (x >= this->x && y >= this->y && x <= this->x + this->width && y <= this->y + this->height);
	}

	/* Returns whether another rectangle intersects this one */
	inline bool intersects(Rect other) {
		return  intersects(other.x, other.y)                              ||
				intersects(other.x + other.width, other.y)                ||
				intersects(other.x + other.width, other.y + other.height) ||
				intersects(other.x, other.y + other.height)               ||
				other.intersects(x, y)                                    ||
				other.intersects(x + width, y)                            ||
				other.intersects(x + width, y + height)                   ||
				other.intersects(x, y + height);
	}

	/* Returns vectors representing corners of the rectangle */
	//Top-left
	Vector2f getVertex1() { return Vector2f(x, y); }
	//Top-right
	Vector2f getVertex2() { return Vector2f(x + width, y); }
	//Bottom-left
	Vector2f getVertex3() { return Vector2f(x, y + height); }
	//Bottom-right
	Vector2f getVertex4() { return Vector2f(x + width, y + height); }
};

#endif /* CORE_RECTANGLE_H_ */
