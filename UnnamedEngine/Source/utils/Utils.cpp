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
		//The root element
		MLElement root("ml");

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
		root.add(window);

		MLElement video("video");
		video.add(MLAttribute("vSync", StrUtils::str(settings.videoVSync)));
		video.add(MLAttribute("samples", StrUtils::str(settings.videoSamples)));
		video.add(MLAttribute("maxAnisotropicSamples", StrUtils::str(settings.videoMaxAnisotropicSamples)));
		video.add(MLAttribute("refreshRate", StrUtils::str(settings.videoRefreshRate)));
		video.add(MLAttribute("resolution", VideoResolution::toString(settings.videoResolution)));
		video.add(MLAttribute("maxFPS", StrUtils::str(settings.videoMaxFPS)));
		root.add(video);

		MLElement audio("audio");
		audio.add(MLAttribute("soundEffectVolume", StrUtils::str(settings.audioSoundEffectVolume)));
		audio.add(MLAttribute("musicVolume", StrUtils::str(settings.audioMusicVolume)));
		root.add(audio);

		MLElement input("input");
		input.add(MLAttribute("repeatKeyboardEvents", StrUtils::str(settings.inputRepeatKeyboardEvents)));
		input.add(MLAttribute("repeatMouseEvents", StrUtils::str(settings.inputRepeatMouseEvents)));
		root.add(input);

		MLElement engine("engine");
		engine.add(MLAttribute("splashScreen", StrUtils::str(settings.engineSplashScreen)));
		root.add(engine);

		MLElement debugging("debugging");
		debugging.add(MLAttribute("showInformation", StrUtils::str(settings.debuggingShowInformation)));
		root.add(debugging);

		//Create the document and save it
		MLDocument document(root);
		document.save(path);
	}

	/* Returns settings read from a file */
	Settings readFromFile(std::string path) {
		//The settings instance
		Settings settings;
		//Load the document
		MLDocument document;
		document.load(path);

		//Go through each child attribute in the document
		for (MLElement& child : document.getRoot().getChildren()) {
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
				}
			}
		}
		//Return the settings
		return settings;
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
			if (std::string(glfwGetJoystickName(i)) == name && name.length() > 0)
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
		if (std::string(glfwGetJoystickName(index)) == name)
			//Return the index as it is correct
			return index;
		else
			//Not the correct controller, so try and locate and return the
			//controller's index
			return getControllerIndexByName(name);
	}
}
