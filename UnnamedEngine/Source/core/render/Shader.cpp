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

#include "Shader.h"

#include <algorithm>
#include <fstream>

#include "../../utils/Logging.h"

/*****************************************************************************
 * The Shader class
 *****************************************************************************/

Shader* Shader::currentShader = NULL;

Shader::Shader(GLint vertexShader, GLint geometryShader, GLint fragmentShader) {
	this->program = glCreateProgram();
	this->vertexShader = vertexShader;
	this->geometryShader = geometryShader;
	this->fragmentShader = fragmentShader;

	attach(vertexShader);
	if (geometryShader)
		attach(geometryShader);
	attach(fragmentShader);
}

Shader::~Shader() {
	destroy();
}

void Shader::attach(GLuint shader) {
	glAttachShader(program, shader);
	glLinkProgram(program);

	//Check for errors
	GLint status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (! status) {
		GLchar error[1024];
		glGetProgramInfoLog(program, sizeof(error), NULL, error);
		Logger::log("Error linking shader" + StrUtils::str(error), "Shader", LogType::Error);
	}

	status = 0;
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (! status) {
		GLchar error[1024];
		glGetProgramInfoLog(program, sizeof(error), NULL, error);
		Logger::log("Error validating shader program " + StrUtils::str(error), "Shader", LogType::Error);
	}
}

void Shader::detach(GLuint shader) {
	glDetachShader(program, shader);
}

void Shader::use() {
	if (currentShader != this) {
		glUseProgram(program);
		currentShader = this;
	}
}

void Shader::stopUsing() {
	glUseProgram(0);
	currentShader = NULL;
}

void Shader::destroy() {
	glDeleteProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::addUniform(std::string id, std::string name) {
	GLint location = glGetUniformLocation(program, name.c_str());
	if (location == -1)
		Logger::log("Could not find uniform with the name '" + name + "'", "Shader", LogType::Warning);
	uniforms.insert(std::pair<std::string, GLint>(id, location));
}

void Shader::addAttribute(std::string id, std::string name) {
	GLint location = glGetAttribLocation(program, name.c_str());
	if (location == -1)
		Logger::log("Could not find attribute with the name '" + name + "'", "Shader", LogType::Warning);
	attributes.insert(std::pair<std::string, GLint>(id, location));
}

GLint Shader::getUniformLocation(std::string id) {
	if (! uniforms.count(id)) {
		//Logger::log("The uniform with the id '" + id + "' was not added", "Shader", Logger::LogType::Debug);
		return -1;
	} else
		return uniforms.at(id);
}

GLint Shader::getAttributeLocation(std::string id) {
	if (! attributes.count(id)) {
		Logger::log("The attribute with the id '" + id + "' was not added", "Shader", LogType::Debug);
		return -1;
	} else
		return attributes.at(id);
}

void Shader::setUniformi(std::string id, GLuint value) {
	glUniform1i(getUniformLocation(id), value);
}

void Shader::setUniformf(std::string id, GLfloat value) {
	glUniform1f(getUniformLocation(id), value);
}

void Shader::setUniformVector2(std::string id, Vector2f value) {
	glUniform2f(getUniformLocation(id), value.getX(), value.getY());
}

void Shader::setUniformVector3(std::string id, Vector3f value) {
	glUniform3f(getUniformLocation(id), value.getX(), value.getY(), value.getZ());
}

void Shader::setUniformVector4(std::string id, Vector4f value) {
	glUniform4f(getUniformLocation(id), value.getX(), value.getY(), value.getZ(), value.getW());
}

void Shader::setUniformColourRGB(std::string id, Colour value) {
	glUniform3f(getUniformLocation(id), value.getR(), value.getG(), value.getB());
}

void Shader::setUniformColourRGBA(std::string id, Colour value) {
	glUniform4f(getUniformLocation(id), value.getR(), value.getG(), value.getB(), value.getA());
}

void Shader::setUniformMatrix3(std::string id, Matrix3f matrix) {
	glUniformMatrix3fv(getUniformLocation(id), 1, GL_FALSE, matrix.front());
}

void Shader::setUniformMatrix4(std::string id, Matrix4f matrix) {
	glUniformMatrix4fv(getUniformLocation(id), 1, GL_FALSE, matrix.front());
}

GLint Shader::createShader(std::string source, GLenum type) {
	GLint shader = glCreateShader(type);
	const GLchar* shaderSource = source.c_str();

	glShaderSource(shader, 1, &shaderSource, NULL); //Length not needed as std::string null terminated
	glCompileShader(shader);

	//Check for errors
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (! status) {
		GLchar error[1024];
		glGetShaderInfoLog(shader, sizeof(error), NULL, error);
		Logger::log("Error compiling shader: " + StrUtils::str(error) + " Source:\n" + source, "Shader", LogType::Error);

		glDeleteShader(shader);

		return -1;
	}
	return shader;
}

Shader* Shader::createShader(ShaderSource vertexSource, ShaderSource fragmentSource) {
	//Create the shader
	Shader* shader = new Shader(Shader::createShader(vertexSource.source, GL_VERTEX_SHADER), Shader::createShader(fragmentSource.source, GL_FRAGMENT_SHADER));

	//Add the uniforms and attributes (if any)
	shader->use();
	for (auto& iterator : vertexSource.uniforms)
		shader->addUniform(iterator.first, iterator.second);
	for (auto& iterator : vertexSource.attributes)
		shader->addAttribute(iterator.first, iterator.second);
	for (auto& iterator : fragmentSource.uniforms)
		shader->addUniform(iterator.first, iterator.second);
	for (auto& iterator : fragmentSource.attributes)
		shader->addAttribute(iterator.first, iterator.second);
	shader->stopUsing();

	//Return the shader
	return shader;
}

Shader* Shader::createShader(ShaderSource vertexSource, ShaderSource geometrySource, ShaderSource fragmentSource) {
	//Create the shader
	Shader* shader = new Shader(Shader::createShader(vertexSource.source, GL_VERTEX_SHADER), Shader::createShader(geometrySource.source, GL_GEOMETRY_SHADER), Shader::createShader(fragmentSource.source, GL_FRAGMENT_SHADER));

	//Add the uniforms and attributes (if any)
	shader->use();
	for (auto& iterator : vertexSource.uniforms)
		shader->addUniform(iterator.first, iterator.second);
	for (auto& iterator : vertexSource.attributes)
		shader->addAttribute(iterator.first, iterator.second);
	for (auto& iterator : geometrySource.uniforms)
		shader->addUniform(iterator.first, iterator.second);
	for (auto& iterator : geometrySource.attributes)
		shader->addAttribute(iterator.first, iterator.second);
	for (auto& iterator : fragmentSource.uniforms)
		shader->addUniform(iterator.first, iterator.second);
	for (auto& iterator : fragmentSource.attributes)
		shader->addAttribute(iterator.first, iterator.second);
	shader->stopUsing();

	//Return the shader
	return shader;
}

Shader::ShaderSource Shader::loadShaderSource(std::string path) {
	//The ShaderSource
	Shader::ShaderSource source;

	//Get the file text
	std::vector<std::string> fileText = FileUtils::readFile(path);

	//Go through each line of file text
	for (unsigned int i = 0; i < fileText.size(); i++) {
		//Check for directives
		if (StrUtils::strStartsWith(fileText[i], "#include ")) {
			//The directory the shader is in
			std::string dir = "";
			size_t pos = path.rfind("/") + 1;
			if (pos != std::string::npos)
				dir = path.substr(0, pos);
			//The file name (removes the "")
			std::string fileName = fileText[i].substr(fileText[i].find("\"") + 1, fileText[i].rfind("\"") - fileText[i].find("\"") - 1);
			//Get the new text
			std::vector<std::string> newText = FileUtils::readFile(dir + fileName);
			//Insert the new text into the file text
			fileText.insert(fileText.begin() + i + 1, newText.begin(), newText.end());
			//Remove the current line
			fileText.erase(fileText.begin() + i);

			i -= 1;
		} else if (StrUtils::strStartsWith(fileText[i], "#map ")) {
			//Split up the line by a space
			std::vector<std::string> line = StrUtils::strSplit(fileText[i], ' ');
			//Check the type and add the values
			if (line[1] == "uniform")
				source.uniforms.insert(std::pair<std::string, std::string>(line[2], line[3]));
			else if (line[1] == "attribute")
				source.attributes.insert(std::pair<std::string, std::string>(line[2], line[3]));

			//Remove the current line
			fileText.erase(fileText.begin() + i);

			i -= 1;
		}
	}

	//Assign the source
	source.source = "";

	//Go through each line of file text
	for (std::string line : fileText)
		//Append the line onto the source
		source.source += line + "\n";

//	std::cout << "----------------------------------------------" << std::endl;
//	std::cout << source.source << std::endl;
//	std::cout << "----------------------------------------------" << std::endl;

	//Return the source
	return source;
}

Shader* Shader::loadShader(std::string path) {
	return createShader(loadShaderSource(path + ".vs"), loadShaderSource(path + ".fs"));
}

/*****************************************************************************
 * The RenderShader class
 *****************************************************************************/

void RenderShader::addForwardShader(Shader* forwardShader) {
	forwardShaders.push_back(forwardShader);
}

void RenderShader::removeForwardShader(Shader* forwardShader) {
	forwardShaders.erase(std::remove(forwardShaders.begin(), forwardShaders.end(), forwardShader), forwardShaders.end());
}

void RenderShader::removeLastForwardShader() {
	forwardShaders.pop_back();
}

Shader* RenderShader::getShader() {
	return forwardShaders.back();
}
