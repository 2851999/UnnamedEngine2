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

#ifndef CORE_RENDER_COLOUR_H_
#define CORE_RENDER_COLOUR_H_

#include "../Vector.h"

/*****************************************************************************
 * The Colour class
 *****************************************************************************/

class Colour : public Vector4f {
public:
	/* Predetermined colours */
	static const Colour NONE;
	static const Colour BLACK;
	static const Colour GREY;
	static const Colour LIGHT_GREY;
	static const Colour RED;
	static const Colour ORANGE;
	static const Colour YELLOW;
	static const Colour PINK;
	static const Colour GREEN;
	static const Colour BLUE;
	static const Colour LIGHT_BLUE;
	static const Colour WHITE;

//	static const Colour ARRAY_RGB[];
//	static const Colour ARRAY_GREY[];
//	static const Colour ARRAY_BLUE[];
//	static const Colour ARRAY_SUNSET[];

	/* The constructors */
	Colour() : Vector4f() {}
	Colour(const Vector<float, 4> &base) : Vector4f(base) {}
	Colour(Colour colour, float a) : Vector4f(colour.getR(), colour.getG(), colour.getB(), a) {}
	Colour(float grey, float a = 1.0f) : Vector4f(grey, grey, grey, a) {}
	Colour(float r, float g, float b, float a = 1.0f) : Vector4f(r, g, b, a) {}

	/* The setters and getterss */
	void set(float grey, float a = 1.0f) {
		setX(grey); setY(grey); setZ(grey); setA(a);
	}

	void set(float r, float g, float b, float a = 1.0f) {
		setX(r); setY(g); setZ(b); setW(a);
	}

	void setR(float r) { setX(r); }
	void setG(float g) { setY(g); }
	void setB(float b) { setZ(b); }
	void setA(float a) { setW(a); }

	float getR() { return getX(); }
	float getG() { return getY(); }
	float getB() { return getZ(); }
	float getA() { return getW(); }
};

#endif /* CORE_RENDER_COLOUR_H_ */
