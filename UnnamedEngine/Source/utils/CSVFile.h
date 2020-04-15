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

#pragma once

#include <string>
#include <vector>

/*****************************************************************************
 * The CSVFile class helps to read from a CSV file
 *****************************************************************************/

class CSVFile {
private:
	/* The path of the file */
	std::string path;

	/* The data within the file */
	std::vector<std::vector<std::string>> data;
public:
	/* Constructor */
	CSVFile() {}

	/* Destructor */
	virtual ~CSVFile() {}

	/* Method to read the file contents */
	void read(const std::string& path);

	/* Method to return the data in a particular row */
	inline unsigned int getRowCount() { return data.size(); }
	inline std::vector<std::string>& getRow(unsigned int index) { return data[index]; }
};

