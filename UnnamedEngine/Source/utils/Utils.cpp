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

#include "Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdarg>

#include "Logging.h"
#include "../core/Window.h"
#include "../core/ml/ML.h"

/*****************************************************************************
 * Various string utilities
 *****************************************************************************/

namespace StrUtils {
	unsigned int strToUInt(const std::string& string) {
		unsigned int value = 0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to unsigned integer", "StrUtils", LogType::Error);
		return value;
	}

	int strToInt(const std::string& string) {
		int value = 0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to integer", "StrUtils", LogType::Error);
		return value;
	}

	float strToFloat(const std::string& string) {
		float value = 0.0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to float", "StrUtils", LogType::Error);
		return value;
	}

	bool strToBool(const std::string& string) {
		bool value = 0;
		std::stringstream ss(string);
		if (! (ss >> value))
			Logger::log("String " + string + " cannot be converted to boolean", "StrUtils", LogType::Error);
		return value;
	}

	std::vector<std::string> strSplit(const std::string &s, char delimeter) {
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> split;
		while (std::getline(ss, item, delimeter))
			split.push_back(item);
		return split;
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

	std::string replaceAll(const std::string &s, const std::string &old, const std::string &replacement) {
		size_t index = 0;
		std::string str = s;
		while ((index = str.find(old, index)) != std::string::npos) {
			str.replace(index, old.length(), replacement);
			index += replacement.length();
		}
		return str;
	}

	std::string remove(const std::string &s, const std::string &substr) {
		size_t start = s.find(substr);
		std::string str = s;

		if (start != std::string::npos)
		   str.erase(start, substr.length());
		return str;
	}
}

/*****************************************************************************
 * Various maths utilities
 *****************************************************************************/

namespace MathsUtils {
	float clampToClosestInterval(float value, float interval) {
		float remainder = fmod(value, interval);
		if (remainder < interval / 2.0f)
			value -= remainder;
		else
			value += (interval - remainder);
		return value;
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
			Logger::log("Could not open file: " + path, "FileUtils", LogType::Error);
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
			Logger::log("Could not open file: " + path, "FileUtils", LogType::Error);

		return output;
	}

	void writeFile(std::string path, std::string text) {
		std::ofstream output;

		output.open(path.c_str());

		if (output.is_open()) {
			output << text;
			output.close();
		} else
			Logger::log("Could not open file: " + path, "FileUtils", LogType::Error);
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
			Logger::log("Could not open file: " + path, "FileUtils", LogType::Error);
	}

	/* Returns whether the file with the specified path exists */
	bool doesExist(std::string path) {
		return boost::filesystem::exists(path.c_str());
	}

	/* Returns whether the specified path is a file */
	bool isFile(std::string path) {
		return boost::filesystem::is_regular_file(path.c_str());
	}

	/* Returns whether the specified path is a directory */
	bool isDirectory(std::string path) {
		return boost::filesystem::is_directory(path.c_str());
	}
}

/*****************************************************************************
 * Various file utilities
 *****************************************************************************/

#include <ctime>

namespace TimeUtils {
	double getSeconds() {
		return glfwGetTime();
	}

	void sleep(long milliseconds) {
		//For Windows
		Sleep(milliseconds);
	}

	std::string getTimeAsString() {
		time_t t = time(NULL);
		struct tm* now = localtime(&t);

		std::string hour = StrUtils::str(now->tm_hour);
		std::string minute = StrUtils::str(now->tm_min);
		std::string second = StrUtils::str(now->tm_sec);

		if (hour.length() == 1)
			hour = "0" + hour;
		if (minute.length() == 1)
			minute = "0" + minute;
		if (second.length() == 1)
			second = "0" + second;

		return hour + ":" + minute + ":" + second;
	}
}

/*****************************************************************************
 * Various random utilities
 *****************************************************************************/

#include <cstdlib>

namespace RandomUtils {
	/* Method used to initialise the random generator with the current time */
	void initialise() {
		std::srand(std::time(0));
	}

	/* Returns a random floating point number between the values specified */
	float randomFloat(float min, float max) {
		return min + ((float) rand()) / (RAND_MAX / (max - min));
	}

	/* Returns a random floating point number between 0 and 1 */
	float randomFloat() {
		return ((float) rand()) / RAND_MAX;
	}
}

/*****************************************************************************
 * Various settings utilities
 *****************************************************************************/

#include "../core/Settings.h"

namespace SettingsUtils {
	/* Writes settings to a document */
	void writeToDocument(MLDocument& document, Settings& settings) {
		//Create the settings element for the settings
		MLElement settingsElement("settings");

		//Add all of the elements
		MLElement window("window");
		window.add(MLAttribute("title", settings.windowTitle));
		window.add(MLAttribute("width", StrUtils::str(settings.windowWidth)));
		window.add(MLAttribute("height", StrUtils::str(settings.windowHeight)));
		window.add(MLAttribute("resizable", StrUtils::str(settings.windowResizable)));
		window.add(MLAttribute("decorated", StrUtils::str(settings.windowDecorated)));
		window.add(MLAttribute("borderless", StrUtils::str(settings.windowBorderless)));
		window.add(MLAttribute("fullscreen", StrUtils::str(settings.windowFullscreen)));
		window.add(MLAttribute("floating", StrUtils::str(settings.windowFloating)));
		settingsElement.add(window);

		MLElement video("video");
		video.add(MLAttribute("vSync", StrUtils::str(settings.videoVSync)));
		video.add(MLAttribute("samples", StrUtils::str(settings.videoSamples)));
		video.add(MLAttribute("maxAnisotropicSamples", StrUtils::str(settings.videoMaxAnisotropicSamples)));
		video.add(MLAttribute("refreshRate", StrUtils::str(settings.videoRefreshRate)));
		video.add(MLAttribute("resolution", VideoResolution::toString(settings.videoResolution)));
		video.add(MLAttribute("maxFPS", StrUtils::str(settings.videoMaxFPS)));
		settingsElement.add(video);

		MLElement audio("audio");
		audio.add(MLAttribute("soundEffectVolume", StrUtils::str(settings.audioSoundEffectVolume)));
		audio.add(MLAttribute("musicVolume", StrUtils::str(settings.audioMusicVolume)));
		settingsElement.add(audio);

		MLElement input("input");
		input.add(MLAttribute("repeatKeyboardEvents", StrUtils::str(settings.inputRepeatKeyboardEvents)));
		input.add(MLAttribute("repeatMouseEvents", StrUtils::str(settings.inputRepeatMouseEvents)));
		settingsElement.add(input);

		MLElement engine("engine");
		engine.add(MLAttribute("splashScreen", StrUtils::str(settings.engineSplashScreen)));
		settingsElement.add(engine);

		MLElement debugging("debugging");
		debugging.add(MLAttribute("showInformation", StrUtils::str(settings.debuggingShowInformation)));
		debugging.add(MLAttribute("consoleEnabled", StrUtils::str(settings.debuggingConsoleEnabled)));
		settingsElement.add(debugging);

		//Add the settings element, or replace the current one if one already exists
		int index = document.getRoot().find("settings");
		if (index >= 0)
			document.getRoot().setChild(index, settingsElement);
		else
			document.getRoot().add(settingsElement);
	}

	/* Writes settings to a file */
	void writeToFile(std::string path, Settings& settings) {
		//Create the document
		MLDocument document(MLElement("ml"));
		//Write the settings to the document
		writeToDocument(document, settings);
		//Save the document
		document.save(path);
	}

	/* Returns settings stored in a document */
	Settings readFromDocument(MLDocument& document) {
		//The settings instance
		Settings settings;
		//Attempt to get the index of the settings element in the document
		int index = document.getRoot().find("settings");
		//Check whether the settings element was found
		if (index >= 0) {
			//Go through each child attribute in the document
			for (MLElement& child : document.getRoot().getChild(index).getChildren()) {
				//Go through the attributes of the child element
				for (MLAttribute& attrib : child.getAttributes()) {
					//Check the child name
					if (child.getName() == "window") {
						//Check the attribute name
						if (attrib.getName() == "title")
							settings.windowTitle = attrib.getData();
						else if (attrib.getName() == "width")
							settings.windowWidth = attrib.getDataAsUInt();
						else if (attrib.getName() == "height")
							settings.windowHeight = attrib.getDataAsUInt();
						else if (attrib.getName() == "resizable")
							settings.windowResizable = attrib.getDataAsBool();
						else if (attrib.getName() == "decorated")
							settings.windowDecorated = attrib.getDataAsBool();
						else if (attrib.getName() == "borderless")
							settings.windowBorderless = attrib.getDataAsBool();
						else if (attrib.getName() == "fullscreen")
							settings.windowFullscreen = attrib.getDataAsBool();
						else if (attrib.getName() == "floating")
							settings.windowFloating = attrib.getDataAsBool();
					} else if (child.getName() == "video") {
						//Check the attribute name
						if (attrib.getName() == "vSync")
							settings.videoVSync = attrib.getDataAsBool();
						else if (attrib.getName() == "samples")
							settings.videoSamples = attrib.getDataAsUInt();
						else if (attrib.getName() == "maxAnisotropicSamples")
							settings.videoMaxAnisotropicSamples = attrib.getDataAsUInt();
						else if (attrib.getName() == "refreshRate")
							settings.videoRefreshRate = attrib.getDataAsUInt();
						else if (attrib.getName() == "resolution")
							settings.videoResolution = VideoResolution::toVector(attrib.getData());
						else if (attrib.getName() == "maxFPS")
							settings.videoMaxFPS = attrib.getDataAsUInt();
					} else if (child.getName() == "audio") {
						//Check the attribute name
						if (attrib.getName() == "soundEffectVolume")
							settings.audioSoundEffectVolume = attrib.getDataAsUInt();
						else if (attrib.getName() == "musicVolume")
							settings.audioMusicVolume = attrib.getDataAsUInt();
					} else if (child.getName() == "input") {
						//Check the attribute name
						if (attrib.getName() == "repeatKeyboardEvents")
							settings.inputRepeatKeyboardEvents = attrib.getDataAsBool();
						else if (attrib.getName() == "repeatMouseEvents")
							settings.inputRepeatMouseEvents = attrib.getDataAsBool();
					} else if (child.getName() == "engine") {
						//Check the attribute name
						if (attrib.getName() == "splashScreen")
							settings.engineSplashScreen = attrib.getDataAsBool();
					} else if (child.getName() == "debugging") {
						//Check the attribute name
						if (attrib.getName() == "showInformation")
							settings.debuggingShowInformation = attrib.getDataAsBool();
						else if (attrib.getName() == "consoleEnabled")
							settings.debuggingConsoleEnabled = attrib.getDataAsBool();
					}
				}
			}
		} else
			//Log an error
			Logger::log("Settings not found in the document", "SettingsUtils", LogType::Error);
		//Return the settings
		return settings;
	}

	/* Returns settings read from a file */
	Settings readFromFile(std::string path) {
		//Load the document
		MLDocument document;
		document.load(path);
		//Return the settings
		return readFromDocument(document);
	}
}

/*****************************************************************************
 * Various clipboard utilities
 *****************************************************************************/

namespace ClipboardUtils {
	void setText(std::string text) {
		glfwSetClipboardString(Window::getCurrentInstance()->getInstance(), text.c_str());
	}

	std::string getText() {
		return std::string(glfwGetClipboardString(Window::getCurrentInstance()->getInstance()));
	}
}

/*****************************************************************************
 * Various controller utilities
 *****************************************************************************/

namespace ControllerUtils {
	/* Returns the index of a controller given its name or -1 if not found */
	int getControllerIndexByName(std::string name) {
		//Go through each possible index
		for (unsigned int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++) {
			//Check the name of the current controller
			if (glfwJoystickPresent(i) && std::string(glfwGetJoystickName(i)) == name && name.length() > 0)
				//Return the index
				return i;
		}
		return -1;
	}

	/* Return the index of a controller if it has the provided name, if not
	 * it will search for a controller with that name and return that or -1
	 * if still not found */
	int findController(int index, std::string name) {
		//Get the name of the controller at the given index
		if (glfwJoystickPresent(index) && std::string(glfwGetJoystickName(index)) == name)
			//Return the index as it is correct
			return index;
		else
			//Not the correct controller, so try and locate and return the
			//controller's index
			return getControllerIndexByName(name);
	}
}
