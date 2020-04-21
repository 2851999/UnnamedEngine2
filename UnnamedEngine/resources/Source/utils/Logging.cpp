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

#include "Logging.h"

#include <fstream>

#include "Utils.h"

bool Logger::saveLogsToFile = false;

std::ofstream Logger::fileOutputStream;

LogType Logger::logLevel = LogType::Debug | LogType::Information | LogType::Warning | LogType::Error;

bool Logger::includeTimeStamp = true;

void Logger::startFileOutput(std::string path) {
	//Attempt to open the output stream
	fileOutputStream.open(path.c_str(), std::ofstream::out | std::ofstream::app);
	//Check whether the output stream was successfully opened
	if (fileOutputStream.is_open())
		saveLogsToFile = true;
	else
		//Log an error
		log("Could not open file: " + path, "Logger", LogType::Error);
}

void Logger::stopFileOutput() {
	//Ensure the output stream is currently open
	if (fileOutputStream.is_open())
		fileOutputStream.close();
}

void Logger::log(std::string message, LogType type) {
	if (shouldLog(type)) {
		std::string m = "[" + logTypeString(type) + "]" + message;
		if (includeTimeStamp)
			m = "[" + utils_time::getTimeAsString() + "]" + m;
		std::cout << m << std::endl;
		//Check whether the same message should be output to a file
		if (saveLogsToFile) {
			//Output the message
			fileOutputStream << m << std::endl;
			fileOutputStream.flush();
		}
	}
}
