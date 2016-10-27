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

#ifndef UTILS_LOGGING_H_
#define UTILS_LOGGING_H_

#include <iostream>

/*****************************************************************************
 * Stuff for logging
 *****************************************************************************/

namespace Logger {
	/* Log types */
	enum LogType {
		Debug       = 1,
		Information = 2,
		Warning     = 4,
		Error       = 8
	};

	/* Provides flag functionality for log levels */
	inline LogType operator|(LogType a, LogType b) {
		return static_cast<LogType>(static_cast<int>(a) | static_cast<int>(b));
	}

	/* Used to get a string representation of a log type */
	inline std::string logTypeString(LogType type) {
		if (type == LogType::Debug)
			return "DEBUG";
		else if (type == LogType::Information)
			return "INFO";
		else if (type == LogType::Warning)
			return "WARNING";
		else if (type == LogType::Error)
			return "ERROR";
		else
			return "UNKNOWN";
	}

	/* The current log level */
	extern LogType logLevel;

	/* Some logger settings that can be modified */
	extern bool includeTimeStamp;

	/* Determines whether a log should be printed based on the current log level */
	inline bool shouldLog(LogType type) { return logLevel & type; }

	/* Various log functions */
	void log(std::string message, LogType type);

	inline void log(std::string message) {
		log(" " + message, LogType::Debug);
	}

	inline void log(std::string message, std::string source, LogType type) {
		log("[" + source + "] " + message, type);
	}

}


#endif /* UTILS_LOGGING_H_ */
