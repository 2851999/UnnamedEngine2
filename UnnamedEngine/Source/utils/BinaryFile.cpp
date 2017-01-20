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

#include "BinaryFile.h"

#include "Logging.h"

/*****************************************************************************
 * The BinaryFile class
 *****************************************************************************/

BinaryFile::BinaryFile(const std::string& path) : path(path) {}

void BinaryFile::open(Operation operation) {
	//Assign the current operation
	this->currentOperation = operation;

	//Check the operation and open the file
	if (isReading()) {
		input.open(path, std::ios::binary);
		//Check whether the file was opened correctly
		if (! input) {
			//An error occurred
			Logger::log("Could not open the file " + path, "BinaryFile", LogType::Error);
			this->currentOperation = Operation::NONE;
		}
	} else if (isWriting()) {
		output.open(path, std::ios::binary);
		//Check whether the file was opened correctly
		if (! input) {
			//An error occurred
			Logger::log("Could not open the file " + path, "BinaryFile", LogType::Error);
			this->currentOperation = Operation::NONE;
		}
	}
}

void BinaryFile::close() {
	//Check the operation and close the file
	if (isReading())
		input.close();
	else if (isWriting())
		output.close();
	//Reset the current operation
	currentOperation = Operation::NONE;
}

void BinaryFile::readUInt(unsigned int& value) {
	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
}

void BinaryFile::readFloat(float& value) {
	input.read(reinterpret_cast<char*>(&value), sizeof(float));
}


void BinaryFile::readVector3f(Vector3f& vector) {
	input.read(reinterpret_cast<char*>(vector.data()), 3 * sizeof(float));
}

void BinaryFile::readVector4f(Vector4f& vector) {
	input.read(reinterpret_cast<char*>(vector.data()), 4 * sizeof(float));
}

void BinaryFile::readMatrix4f(Matrix4f& matrix) {
	input.read(reinterpret_cast<char*>(matrix.data()), 16 * sizeof(float));
}

void BinaryFile::readString(std::string& string) {
	size_t length;
	input.read(reinterpret_cast<char*>(&length), sizeof(size_t));
	char* temp = new char[length + 1];
	input.read(temp, length);
	temp[length] = '\0';
	string = temp;
	delete [] temp;
}

void BinaryFile::readVectorDataFloat(std::vector<float>& data) {
	unsigned int numItems = data.size();
	readUInt(numItems);
	if (numItems > 0) {
		data.resize(numItems);
		input.read(reinterpret_cast<char*>(data.data()), numItems * sizeof(float));
	}
}

void BinaryFile::readVectorDataUInt(std::vector<unsigned int>& data) {
	unsigned int numItems = data.size();
	readUInt(numItems);
	if (numItems > 0) {
		data.resize(numItems);
		input.read(reinterpret_cast<char*>(data.data()), numItems * sizeof(unsigned int));
	}
}

void BinaryFile::writeUInt(unsigned int value) {
	output.write(reinterpret_cast<char*>(&value), sizeof(unsigned int));
}

void BinaryFile::writeFloat(float value) {
	output.write(reinterpret_cast<char*>(&value), sizeof(float));
}

void BinaryFile::writeVector3f(Vector3f vector) {
	output.write(reinterpret_cast<char*>(vector.data()), 3 * sizeof(float));
}

void BinaryFile::writeVector4f(Vector4f vector) {
	output.write(reinterpret_cast<char*>(vector.data()), 4 * sizeof(float));
}

void BinaryFile::writeMatrix4f(Matrix4f matrix) {
	output.write(reinterpret_cast<char*>(matrix.data()), 16 * sizeof(float));
}

void BinaryFile::writeString(std::string string) {
	size_t length = string.size();
	output.write(reinterpret_cast<char*>(&length), sizeof(size_t));
	output.write(string.c_str(), length);
}

void BinaryFile::writeVectorDataFloat(std::vector<float>& data) {
	unsigned int numItems = data.size();
	writeUInt(numItems);
	if (numItems > 0)
		output.write(reinterpret_cast<char*>(data.data()), numItems * sizeof(float));
}

void BinaryFile::writeVectorDataUInt(std::vector<unsigned int>& data) {
	unsigned int numItems = data.size();
	output.write(reinterpret_cast<char*>(&numItems), sizeof(unsigned int));
	if (numItems > 0)
		output.write(reinterpret_cast<char*>(data.data()), numItems * sizeof(unsigned int));
}
