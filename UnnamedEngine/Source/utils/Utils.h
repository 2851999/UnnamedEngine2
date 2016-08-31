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

#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <sstream>
#include <vector>

/*****************************************************************************
 * Various string utilities
 *****************************************************************************/

namespace StrUtils {

	template<typename T>
	inline std::string str(T value) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	inline bool strStartsWith(std::string value, std::string prefix) {
		return value.length() >= prefix.length() && value.compare(0, prefix.length(), prefix) == 0;
	}

	inline bool strEndsWith(std::string value, std::string suffix) {
		return value.length() >= suffix.length() && value.compare(value.length() - suffix.length(), suffix.length(), suffix) == 0;
	}

	inline std::vector<std::string> strSplit(const std::string &s, char delimeter) {
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> split;
		while (std::getline(ss, item, delimeter))
			split.push_back(item);
		return split;
	}
};

#include <cmath>

/*****************************************************************************
 * Various maths utilities
 *****************************************************************************/

namespace MathsUtils {
	const float PI = 3.14159265;

	inline float toRadians(float degrees) {
		return degrees * (PI / 180.0f);
	}

	inline float toDegrees(float radians) {
		return radians * (180.0f / PI);
	}

	inline float clamp(float value, float min, float max) {
		if (value < min)
			return min;
		else if (value > max)
			return max;
		else
			return value;
	}

	template<typename T>
	inline T min(T a, T b) {
		if (a < b)
			return a;
		else
			return b;
	}

	template<typename T>
	inline T max(T a, T b) {
		if (a > b)
			return a;
		else
			return b;
	}
}

/*****************************************************************************
 * Various OpenGL utilities
 *****************************************************************************/

#include <GL/glew.h>

namespace GLUtils {
	inline unsigned int glValue(bool value) {
		if (value)
			return GL_TRUE;
		else
			return GL_FALSE;
	}

	inline bool boolValue(int value) {
		return value == GL_TRUE;
	}
}

/*****************************************************************************
 * Various file utilities
 *****************************************************************************/

#include <vector>

namespace FileUtils {
	/* Reads the contents of a file to a string - useful for shaders */
	std::string readFileToString(std::string path);

	/* Reads the contents of a file to a vector */
	std::vector<std::string> readFile(std::string path);

	/* Writes to a file given the text to be written in the form of a string */
	void writeFile(std::string path, std::string text);

	/* Writes to a file given the text to be written in the form of a vector */
	void writeFile(std::string path, std::vector<std::string> text);
}

/*****************************************************************************
 * Various time utilities
 *****************************************************************************/

namespace TimeUtils {
	/* Returns the time in seconds since the active window was created */
	double getSeconds();

	/* Returns the time elapsed in milliseconds (rounded) since the GLFW was initialised */
	inline long getMilliseconds() { return (long) (getSeconds() * 1000); }

	/* Pauses the thread for a set amount of time given in milliseconds */
	void sleep(long milliseconds);
}

/*****************************************************************************
 * Various random utilities
 *****************************************************************************/

namespace RandomUtils {
	/* Returns a random floating point number between the values specified */
	inline float randomFloat(float min, float max) { return min + ((float) rand()) / (RAND_MAX / (max - min)); }

	/* Returns a random floating point number between 0 and 1 */
	inline float randomFloat() { return ((float) rand()) / RAND_MAX; }
}

#endif /* UTILS_UTILS_H_ */
