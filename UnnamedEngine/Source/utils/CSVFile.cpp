/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#include "CSVFile.h"
#include "Utils.h"

/*****************************************************************************
 * The CSVFile class
 *****************************************************************************/

void CSVFile::read(const std::string& path) {
	this->path = path;

	//Read the data from the file
	std::vector<std::string> fileText = utils_file::readFile(path);
	data.resize(fileText.size());

	//Go through and split up data by commas, appending the result to the data
	for (unsigned int i = 0; i < fileText.size(); ++i) {
		std::vector<std::string> lineData = utils_string::strSplit(fileText[i], ',');

		unsigned int j = 0;
		while (j < lineData.size()) {
			//Value to be added at the current line
			std::string value;
			//Check for " at the start
			if (utils_string::strStartsWith(lineData[j], "\"")) {
				value = lineData[j].substr(1, lineData[j].length() - 1);
				//Keep going until the other " is found
				while (! utils_string::strEndsWith(value, "\"")) {
					++j;
					value += utils_string::str(",") + lineData[j];
				}
				//Remove the final "
				value = value.substr(0, value.length() - 1);
			} else
				value = lineData[j];
			//Append the value
			data[i].push_back(value);

			++j;
		}
	}
}
