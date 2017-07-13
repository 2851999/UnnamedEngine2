/*****************************************************************************
 *
 *   Copyright 2017 Joel Davies
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

#ifndef EXPERIMENTAL_TERRAIN_SIMPLEXNOISE_H_
#define EXPERIMENTAL_TERRAIN_SIMPLEXNOISE_H_

//http://weber.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java

class SimplexNoise {
private:
	class Grad {
	public:
		double x;
		double y;
		double z;
		double w;

		Grad(double x, double y, double z) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = 0;
		}

		Grad(double x, double y, double z, double w) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
	};

	static Grad grad3[];

	static short p[];
	short perm[512];
	short permMod12[512];

	static const double F2;
	static const double G2;
	static const double F3;
	static const double G3;
	static const double F4;
	static const double G4;

	inline static int fastFloor(double x) {
		int xi = (int) x;
		return x < xi ? xi - 1 : xi;
	}

	inline static double dot(Grad g, double x, double y) {
		return g.x * x + g.y * y;
	}
public:
	SimplexNoise();
	virtual ~SimplexNoise() {}

	double noise(double xin, double yin);
};

#endif /* EXPERIMENTAL_TERRAIN_SIMPLEXNOISE_H_ */
