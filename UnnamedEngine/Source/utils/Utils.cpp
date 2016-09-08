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

#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <fstream>
#include <cstdarg>
#include "Utils.h"
#include "Logging.h"

/*****************************************************************************
 * Various file utilities
 *****************************************************************************/

namespace FileUtils {
	std::string readFileToString(std::string path) {
		std::ifstream input;
		std::string   line;
		std::string   output;

		input.open(path.c_str());

		if (input.is_open()) {
			while (getline(input, line))
				output += line + "\n";
			input.close();
		} else
			Logger::log("Could not open file: " + path, "FileUtils", Logger::Error);
		return output;
	}

	std::vector<std::string> readFile(std::string path) {
		std::ifstream            input;
		std::string              line;
		std::vector<std::string> output;

		input.open(path.c_str());

		if (input.is_open()) {
			while (getline(input, line))
				output.push_back(line);
			input.close();
		} else
			Logger::log("Could not open file: " + path, "FileUtils", Logger::Error);

		return output;
	}

	void writeFile(std::string path, std::string text) {
		std::ofstream output;

		output.open(path.c_str());

		if (output.is_open()) {
			output << text;
			output.close();
		} else
			Logger::log("Could not open file: " + path, "FileUtils", Logger::Error);
	}

	void writeFile(std::string path, std::vector<std::string> text) {
		std::ofstream output;

		output.open(path.c_str());

		if (output.is_open()) {
			for (unsigned int i = 0; i < text.size(); i++) {
				output << text.at(i);

				//Start a new line unless the current line is the last
				if (i < text.size() - 1)
					output << "\n";
			}
			output.close();
		} else
			Logger::log("Could not open file: " + path, "FileUtils", Logger::Error);
	}
}

/*****************************************************************************
 * Various file utilities
 *****************************************************************************/

namespace TimeUtils {
	double getSeconds() {
		return glfwGetTime();
	}

	void sleep(long milliseconds) {
		//For Windows
		Sleep(milliseconds);
	}
}
