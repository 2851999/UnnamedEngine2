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

	unsigned int strToUInt(const std::string& string);
	int strToInt(const std::string& string);
	float strToFloat(const std::string& string);
	bool strToBool(const std::string& string);

	inline bool strStartsWith(std::string value, std::string prefix) {
		return value.length() >= prefix.length() && value.compare(0, prefix.length(), prefix) == 0;
	}

	inline bool strEndsWith(std::string value, std::string suffix) {
		return value.length() >= suffix.length() && value.compare(value.length() - suffix.length(), suffix.length(), suffix) == 0;
	}

	std::vector<std::string> strSplit(const std::string &s, char delimeter);
	std::vector<std::string> strSplit(const std::string& text, const std::string& delimeter);

	inline std::string substring(const std::string &s, int begin, int end) {
		return s.substr(begin, end - begin);
	}

	std::string replaceAll(const std::string &s, const std::string &old, const std::string &replacement);
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

	template<typename T>
	inline T clamp(T value, T min, T max) {
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

	template<typename T>
	inline T abs(T value) {
		if (value < 0)
			return -value;
		else
			return value;
	}

	/* Returns a value that is a multiple of the interval, closest to the
	 * value given */
	float clampToClosestInterval(float value, float interval);
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

	/* Returns whether the file with the specified path exists */
	bool doesExist(std::string path);

	/* Returns whether the specified path is a file */
	bool isFile(std::string path);

	/* Returns whether the specified path is a directory */
	bool isDirectory(std::string path);
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

	/* Returns the current time as a string */
	std::string getTimeAsString();
}

/*****************************************************************************
 * Various random utilities
 *****************************************************************************/

namespace RandomUtils {
	/* Method used to initialise the random generator with the current time */
	void initialise();

	/* Returns a random floating point number between the values specified */
	float randomFloat(float min, float max);

	/* Returns a random floating point number between 0 and 1 */
	float randomFloat();
}

/*****************************************************************************
 * Various settings utilities
 *****************************************************************************/

class Settings;

namespace SettingsUtils {
	/* Writes settings to a file */
	void writeToFile(std::string path, Settings& settings);

	/* Returns settings read from a file */
	Settings readFromFile(std::string path);
}

/*****************************************************************************
 * Various clipboard utilities
 *****************************************************************************/

namespace ClipboardUtils {
	/* Sets the clipboard text */
	void setText(std::string text);

	/* Returns the current clipboard text */
	std::string getText();
}

/*****************************************************************************
 * Various controller utilities
 *****************************************************************************/

namespace ControllerUtils {
	/* Returns the index of a controller given its name or -1 if not found */
	int getControllerIndexByName(std::string name);

	/* Return the index of a controller if it has the provided name, if not
	 * it will search for a controller with that name and return that or -1
	 * if still not found */
	int findController(int index, std::string name);
}

#endif /* UTILS_UTILS_H_ */
