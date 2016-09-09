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
 * Various string utilities
 *****************************************************************************/

namespace StrUtils {
	unsigned int strToUInt(const std::string& string) {
		unsigned int value = 0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to unsigned integer", "StrUtils", Logger::Error);
		return value;
	}

	int strToInt(const std::string& string) {
		int value = 0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to integer", "StrUtils", Logger::Error);
		return value;
	}

	float strToFloat(const std::string& string) {
		float value = 0.0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to float", "StrUtils", Logger::Error);
		return value;
	}

	bool strToBool(const std::string& string) {
		bool value = 0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to boolean", "StrUtils", Logger::Error);
		return value;
	}

	std::vector<std::string> strSplit(const std::string& text, const std::string& delimeter) {
		std::vector<std::string> split;

		//The location of the delimter found
		size_t loc;

		//The location of the last delimiter found
		size_t last = 0;

		while ((loc = text.find(delimeter, last)) != std::string::npos) {
			//Add the previous text
			split.push_back(text.substr(last, loc - last));
			//Set the last position (But prevent finding the same one by adding
			//the length of the delemiter to the location)
			last = loc + delimeter.size();
		}

		//Add the final part
		split.push_back(text.substr(last));

		return split;
	}
}

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

/*****************************************************************************
 * Various settings utilities
 *****************************************************************************/

#include "../core/Settings.h"

namespace SettingsUtils {
	/* Writes settings to a file */
	void writeToFile(std::string path, Settings& settings) {
		FileUtils::writeFile(path, std::vector<std::string> {
			"WindowTitle: "                + settings.windowTitle,
			"WindowWidth: "                + StrUtils::str(settings.windowWidth),
			"WindowHeight: "               + StrUtils::str(settings.windowHeight),
			"WindowResizable: "            + StrUtils::str(settings.windowResizable),
			"WindowDecorated: "            + StrUtils::str(settings.windowDecorated),
			"WindowBorderless: "           + StrUtils::str(settings.windowBorderless),
			"WindowFullscreen: "           + StrUtils::str(settings.windowFullscreen),
			"WindowFloating: "             + StrUtils::str(settings.windowFloating),
			"VideoVSync: "                 + StrUtils::str(settings.videoVSync),
			"VideoSamples: "               + StrUtils::str(settings.videoSamples),
			"VideoMaxAnisotropicSamples: " + StrUtils::str(settings.videoMaxAnisotropicSamples),
			"VideoRefreshRate: "           + StrUtils::str(settings.videoRefreshRate),
			"VideoResolution: "            + VideoResolution::toString(settings.videoResolution),
			"VideoMaxFPS: "                + StrUtils::str(settings.videoMaxFPS),
			"AudioSoundEffectVolume: "     + StrUtils::str(settings.audioSoundEffectVolume),
			"AudioMusicVolume: "           + StrUtils::str(settings.audioMusicVolume),
			"InputRepeatKeyboardEvents: "  + StrUtils::str(settings.inputRepeatKeyboardEvents),
			"InputRepeatMouseEvents: "     + StrUtils::str(settings.inputRepeatMouseEvents),
			"EngineSplashScreen: "         + StrUtils::str(settings.engineSplashScreen),
			"DebuggingShowInformation: "   + StrUtils::str(settings.debuggingShowInformation)
		});
	}

	/* Returns settings loaded from a file */
	Settings readFromFile(std::string path) {
		Settings settings;

		std::vector<std::string> text = FileUtils::readFile(path);

		//Go through the text
		for (unsigned int i = 0; i < text.size(); i++) {
			//Split up the current line
			std::vector<std::string> line = StrUtils::strSplit(text[i], ": ");
			//Ensure the right number of parameters
			if (line.size() == 2) {
				//Check the first value and assign the appropriate settings variable

				if (line[0] == "WindowTitle")
					settings.windowTitle = line[1];
				else if (line[0] == "WindowWidth")
					settings.windowWidth = StrUtils::strToUInt(line[1]);
				else if (line[0] == "WindowHeight")
					settings.windowHeight = StrUtils::strToUInt(line[1]);
				else if (line[0] == "WindowResizable")
					settings.windowResizable = StrUtils::strToBool(line[1]);
				else if (line[0] == "WindowDecorated")
					settings.windowDecorated = StrUtils::strToBool(line[1]);
				else if (line[0] == "WindowBorderless")
					settings.windowBorderless = StrUtils::strToBool(line[1]);
				else if (line[0] == "WindowFullscreen")
					settings.windowFullscreen = StrUtils::strToBool(line[1]);
				else if (line[0] == "WindowFloating")
					settings.windowFloating = StrUtils::strToBool(line[1]);
				else if (line[0] == "VideoVSync")
					settings.videoVSync = StrUtils::strToBool(line[1]);
				else if (line[0] == "VideoSamples")
					settings.videoSamples = StrUtils::strToUInt(line[1]);
				else if (line[0] == "VideoMaxAnisotropicSamples")
					settings.videoMaxAnisotropicSamples = StrUtils::strToUInt(line[1]);
				else if (line[0] == "VideoRefreshRate")
					settings.videoRefreshRate = StrUtils::strToUInt(line[1]);
				else if (line[0] == "VideoResolution")
					settings.videoResolution = VideoResolution::toVector(line[1]);
				else if (line[0] == "VideoMaxFPS")
					settings.videoMaxFPS = StrUtils::strToUInt(line[1]);
				else if (line[0] == "AudioSoundEffectVolume")
					settings.audioSoundEffectVolume = StrUtils::strToUInt(line[1]);
				else if (line[0] == "AudioMusicVolume")
					settings.audioMusicVolume = StrUtils::strToUInt(line[1]);
				else if (line[0] == "InputRepeatKeyboardEvents")
					settings.inputRepeatKeyboardEvents = StrUtils::strToBool(line[1]);
				else if (line[0] == "InputRepeatMouseEvents")
					settings.inputRepeatMouseEvents = StrUtils::strToBool(line[1]);
				else if (line[0] == "EngineSplashScreen")
					settings.engineSplashScreen = StrUtils::strToBool(line[1]);
				else if (line[0] == "DebuggingShowInformation")
					settings.debuggingShowInformation = StrUtils::strToBool(line[1]);
 			} else
 				Logger::log("Unknown format in Settings file " + path + " line " + StrUtils::str(i), "SettingsUtils", Logger::Error);
		}

		return settings;
	}
}
