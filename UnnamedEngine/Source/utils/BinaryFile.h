/*****************************************************************************
 *
 *   Copyright 2017 Joel Davies
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

#ifndef UTILS_BINARYFILE_H_
#define UTILS_BINARYFILE_H_

#include <string>
#include <vector>
#include <fstream>

#include "../core/Vector.h"
#include "../core/Matrix.h"

/*****************************************************************************
 * The BianryFile class helps to read/write from/to a binary file
 *****************************************************************************/

class BinaryFile {
private:
	/* The path if the file */
	std::string path;

	/* The input/output streams */
	std::ifstream input;
	std::ofstream output;
public:
	/* The type of operation being performed on this file */
	enum Operation {
		NONE,
		READ,
		WRITE
	};

	/* The constructor */
	BinaryFile(const std::string& path);

	/* The destructor */
	virtual ~BinaryFile() {}

	/* Method called to open this file to do a particular operation (Should be closed afterwards) */
	void open(Operation operation);

	/* Method called to close this file */
	void close();

	/* Various reading methods */
	void readUInt(unsigned int& value);
	void readFloat(float& value);
	void readVector3f(Vector3f& vector);
	void readVector4f(Vector4f& vector);
	void readMatrix4f(Matrix4f& matrix);
	void readString(std::string& string);
	void readVectorDataFloat(std::vector<float>& data);
	void readVectorDataUInt(std::vector<unsigned int>& data);

	/* Various writing methods */
	void writeUInt(unsigned int value);
	void writeFloat(float value);
	void writeVector3f(Vector3f vector);
	void writeVector4f(Vector4f vector);
	void writeMatrix4f(Matrix4f matrix);
	void writeString(std::string string);
	void writeVectorDataFloat(std::vector<float>& data);
	void writeVectorDataUInt(std::vector<unsigned int>& data);

	/* Returns various states of this file */
	bool isReading() { return currentOperation == Operation::READ; }
	bool isWriting() { return currentOperation == Operation::WRITE; }
	bool isOpen()    { return currentOperation != Operation::NONE; }
private:
	/* The current operation of this file */
	Operation currentOperation = Operation::NONE;
};


#endif /* UTILS_BINARYFILE_H_ */
