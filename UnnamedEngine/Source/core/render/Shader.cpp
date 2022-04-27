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

#include "../BaseEngine.h"
#include "../vulkan/Vulkan.h"
#include "../../utils/Logging.h"

/*****************************************************************************
 * The Shader class
 *****************************************************************************/

Shader* Shader::currentShader = NULL;

Shader::Shader(GLint vertexShader, GLint geometryShader, GLint fragmentShader) {
	this->program = glCreateProgram();

	attach(vertexShader);
	if (geometryShader > 0)
		attach(geometryShader);
	attach(fragmentShader);
}

Shader::Shader(VkShaderModule vertexShaderModule, VkShaderModule geometryShaderModule, VkShaderModule fragmentShaderModule) {
	attach(vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
	if (geometryShaderModule != VK_NULL_HANDLE)
		attach(geometryShaderModule, VK_SHADER_STAGE_GEOMETRY_BIT);
	attach(fragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);
}

Shader::~Shader() {
	if (! BaseEngine::usingVulkan()) {
		glDeleteProgram(program);
		//Go through each attached shader and delete them
		for (unsigned int i = 0; i < attachedShaders.size(); i++)
			glDeleteShader(attachedShaders[i]);
		attachedShaders.clear();
	} else {
		//Destroy the shader modules
		for (VulkanShaderModule& shaderModule : vulkanShaderModules)
			vkDestroyShaderModule(Vulkan::getDevice()->getLogical(), shaderModule.shaderModule, nullptr);
	}
}

void Shader::attach(GLuint shader) {
	//Add the shader
	attachedShaders.push_back(shader);

	glAttachShader(program, shader);
	glLinkProgram(program);

	//Check for errors
	GLint status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (! status) {
		GLchar error[1024];
		glGetProgramInfoLog(program, sizeof(error), NULL, error);
		Logger::log("Error linking shader" + utils_string::str(error), "Shader", LogType::Error);
	}

	status = 0;
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	if (! status) {
		GLchar error[1024];
		glGetProgramInfoLog(program, sizeof(error), NULL, error);
		Logger::log("Error validating shader program " + utils_string::str(error), "Shader", LogType::Error);
	}
}

void Shader::attach(VkShaderModule shaderModule, VkShaderStageFlagBits shaderStageFlags) {
	vulkanShaderModules.push_back({ shaderModule, shaderStageFlags });
}

void Shader::detach(GLuint shader) {
	//Remove the shader
	attachedShaders.erase(std::remove(attachedShaders.begin(), attachedShaders.end(), shader), attachedShaders.end());

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

void Shader::addUniform(std::string id, std::string name) {
	GLint location = glGetUniformLocation(program, name.c_str());
	//if (location == -1)
		//Logger::log("Could not find uniform with the name '" + name + "'", "Shader", LogType::Warning);
	if (uniforms.find(id) == uniforms.end())
		uniforms.insert(std::pair<std::string, GLint>(id, location));
	else
		uniforms[id] = location;
}

void Shader::addAttribute(std::string id, std::string name) {
	GLint location = glGetAttribLocation(program, name.c_str());
	//if (location == -1)
		//Logger::log("Could not find attribute with the name '" + name + "'", "Shader", LogType::Warning);
	if (attributes.find(id) == attributes.end())
		attributes.insert(std::pair<std::string, GLint>(id, location));
	else
		attributes[id] = location;
}

GLint Shader::getUniformLocation(std::string id) {
	auto uniformIt = uniforms.find(id);
	if (uniformIt == uniforms.end()) {
		//Logger::log("The uniform with the id '" + id + "' was not added", "Shader", Logger::LogType::Debug);
		return -1;
	} else
		return uniformIt->second;
}

GLint Shader::getAttributeLocation(std::string id) {
	auto attributeIt = attributes.find(id);
	if (attributeIt == attributes.end()) {
		Logger::log("The attribute with the id '" + id + "' was not added", "Shader", LogType::Debug);
		return -1;
	} else
		return attributeIt->second;
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
		Logger::log("Error compiling shader: " + utils_string::str(error) + " Source:\n" + source, "Shader", LogType::Error);

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

VkShaderModule Shader::createVkShaderModule(const std::vector<char>& code) {
	//Assign the creation info
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

	//Load the shader module
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(Vulkan::getDevice()->getLogical(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		Logger::log("Failed to create shader module", "VulkanGraphicsPipeline", LogType::Error);

	return shaderModule;
}

std::vector<char> Shader::readFile(const std::string& path) {
	std::ifstream file(path, std::ios::ate | std::ios::binary); //Read at end as can tell size of file from position

	if (! file.is_open())
		Logger::log("Failed to open file " + path, "Shader", LogType::Error);

	size_t fileSize = (size_t) file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

std::vector<std::string> Shader::loadInclude(std::string path, std::string line) {
	//The directory the shader is in
	std::string dir = "";
	size_t pos = path.rfind("/") + 1;
	if (pos != std::string::npos)
		dir = path.substr(0, pos);

	//The relative file path (removes the "")
	std::string filePath = line.substr(line.find("\"") + 1, line.rfind("\"") - line.find("\"") - 1);

	while (utils_string::strStartsWith(filePath, "../")) {
		filePath = utils_string::remove(filePath, "../");

		//Remove last /
		dir = dir.substr(0, dir.length() - 1);
		//Remove last directory
		dir = dir.substr(0, dir.rfind("/") + 1);
	}

	//Get the new text
	std::vector<std::string> newText = utils_file::readFile(dir + filePath);

	return newText;
}

void Shader::loadShaderSource(std::string path, std::vector<std::string> &fileText, ShaderSource &source, unsigned int uboBindingOffset) {
	//Go through each line of file text
	for (unsigned int i = 0; i < fileText.size(); i++) {
		//Check for directives
		if (utils_string::strStartsWith(fileText[i], "#include ")) {

			std::string dir = "";
			size_t pos = path.rfind("/") + 1;
			if (pos != std::string::npos)
				dir = path.substr(0, pos);

			//The relative file path (removes the "")
			std::string filePath = fileText[i].substr(fileText[i].find("\"") + 1, fileText[i].rfind("\"") - fileText[i].find("\"") - 1);

			while (utils_string::strStartsWith(filePath, "../")) {
				filePath = utils_string::remove(filePath, "../");

				//Remove last /
				dir = dir.substr(0, dir.length() - 1);
				//Remove last directory
				dir = dir.substr(0, dir.rfind("/") + 1);
			}

			//Load the new text
			std::vector<std::string> newText = utils_file::readFile(dir + filePath);
			loadShaderSource(dir + filePath, newText, source);

			//Insert the new text into the file text
			fileText.insert(fileText.begin() + i + 1, newText.begin(), newText.end());
			//Remove the current line
			fileText.erase(fileText.begin() + i);

			i -= 1;
		} else if (utils_string::strStartsWith(fileText[i], "#map ")) {
			//Split up the line by a space
			std::vector<std::string> line = utils_string::strSplit(fileText[i], ' ');
			//Check the type and add the values
			if (line[1] == "uniform")
				source.uniforms.insert(std::pair<std::string, std::string>(line[2], line[3]));
			else if (line[1] == "attribute")
				source.attributes.insert(std::pair<std::string, std::string>(line[2], line[3]));

			//Remove the current line
			fileText.erase(fileText.begin() + i);

			i -= 1;
		} else if (uboBindingOffset > 0 && utils_string::strStartsWith(fileText[i], "layout(std140, binding = ")) {
			//Want to apply an offset to the UBO location

			//Split up the line by a space
			std::vector<std::string> line = utils_string::strSplit(fileText[i], ' ');

			//Add the offset
			line[3] = utils_string::str(utils_string::strToUInt(utils_string::remove(line[3], ")")) + uboBindingOffset) + ")";

			//Assign the new line of text
			fileText[i] = utils_string::strJoin(line, ' ');
		} else if (utils_string::strStartsWith(fileText[i], "layout(std140, set = ")) {
			//Want to apply an offset to the UBO location

			//Split up the line by a space
			std::vector<std::string> line = utils_string::strSplit(fileText[i], ' ');

			//Apply the offset if needed
			if (uboBindingOffset > 0)
				//Add the offset
				line[6] = utils_string::str(utils_string::strToUInt(utils_string::remove(line[6], ")")) + uboBindingOffset) + ")";

			if (! BaseEngine::usingVulkan()) {
				//Need to remove 'set =' part
				line[1] = "";
				line[2] = "";
				line[3] = "";
			}

			//Assign the new line of text
			fileText[i] = utils_string::strJoin(line, ' ');
		} else if (utils_string::strStartsWith(fileText[i], "layout(set = ") && ! (BaseEngine::usingVulkan())) {
			//Remove 'set = x' part
			unsigned int startIndex = fileText[i].find("(") + 1;
			fileText[i] = fileText[i].erase(startIndex, fileText[i].find(",") + 2 - startIndex);
		}
	}
}

Shader::ShaderSource Shader::loadShaderSource(std::string path, unsigned int uboBindingOffset, std::string preSource) {
	//The ShaderSource
	Shader::ShaderSource source;

	std::vector<std::string> fileText = utils_file::readFile(path);

	loadShaderSource(path, fileText, source, uboBindingOffset);

	//Assign the source
	source.source = "";

	bool hasVersion = false;
	bool added = false;

	//Go through each line of file text
	for (std::string line : fileText) {
		if (hasVersion && ! added) {
			source.source += preSource + "\n";
			added = true;
		}  else if ((! hasVersion) && utils_string::strStartsWith(line, "#version"))
			hasVersion = true;
		else if (line != "")
			hasVersion = true; //Assume has no version

		//Only append onto the source if it does not define another version (so that the first occrurance is treated as
		//the one that should be used - in the case of includes this means versions before includes take precedence)
		if (! (hasVersion && added && utils_string::strStartsWith(line, "#version")))
			//Append the line onto the source
			source.source += line + "\n";
		//else
		//	std::cout << line << std::endl;
	}

//	std::cout << "----------------------------------------------" << std::endl;
//	std::cout << source.source << std::endl;
//	std::cout << "----------------------------------------------" << std::endl;

	//Return the source
	return source;
}

Shader* Shader::loadShader(std::string path, std::vector<std::string> defines) {
	//The extra code to add on at the start
	std::string preSource = "";

	for (std::string& define : defines)
		preSource += "#define " + define + "\n";

	//Extra part of file name (for Vulkan)
	std::string extraName = "";
	if (defines.size() > 0)
		extraName += "_" + utils_string::strJoin(defines, "");

	//Check whether using Vulkan
	if (! BaseEngine::usingVulkan()) {
		//Check for geometry shader
		if (utils_file::isFile(path + ".gs"))
			return createShader(loadShaderSource(path + ".vs", 0, preSource), loadShaderSource(path + ".gs"), loadShaderSource(path + ".fs", 0, preSource));
		else
			return createShader(loadShaderSource(path + ".vs", 0, preSource), loadShaderSource(path + ".fs", 0, preSource));
	} else {
		//Check for geometry shader
		if (utils_file::isFile(path + "_geom.spv"))
			return new Shader(createVkShaderModule(readFile(path + extraName + "_vert.spv")), createVkShaderModule(readFile(path + extraName + "_geom.spv")), createVkShaderModule(readFile(path + extraName + "_frag.spv")));
		else
			return new Shader(createVkShaderModule(readFile(path + extraName + "_vert.spv")), createVkShaderModule(readFile(path + extraName + "_frag.spv")));
	}
}

Shader* Shader::loadShaderNames(std::string path, std::vector<std::string> fileNames, std::vector<std::string> defines) {
	//The extra code to add on at the start
	std::string preSource = "";

	for (std::string& define : defines)
		preSource += "#define " + define + "\n";

	//Check whether using Vulkan
	if (! BaseEngine::usingVulkan()) {
		Logger::log("You should not use loadShader(std::string path, std::vector<std::string> fileNames, std::vector<std::string> define) outside of Vulkan", "Shader", LogType::Error);
	} else {
		//Create the shader
		Shader* shader = new Shader();

		//Add the modules
		for (std::string fileName : fileNames) {
			//Figure out the type based on the end of the file name
			VkShaderStageFlagBits shaderStageFlags;

			if (utils_string::strEndsWith(fileName, "_vert"))
				shaderStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			else if (utils_string::strEndsWith(fileName, "_frag"))
				shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			else if (utils_string::strEndsWith(fileName, "_geom"))
				shaderStageFlags = VK_SHADER_STAGE_GEOMETRY_BIT;
			else if (utils_string::strEndsWith(fileName, ".rgen"))
				shaderStageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;
			else if (utils_string::strEndsWith(fileName, ".rmiss"))
				shaderStageFlags = VK_SHADER_STAGE_MISS_BIT_KHR;
			else if (utils_string::strEndsWith(fileName, ".rchit"))
				shaderStageFlags = VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
			else
				Logger::log("Unsupported file extension for '" + fileName + "'", "Shader", LogType::Error);

			//Attach the shader module
			shader->attach(createVkShaderModule(readFile(path + fileName + ".spv")), shaderStageFlags);
		}

		return shader;
	}
	return NULL;
}

Shader* Shader::loadEngineShader(std::string path, std::vector<std::string> defines) {
	if (! BaseEngine::usingVulkan())
		return loadShader("resources/shaders/" + path, defines);
	else
		return loadShader("resources/shaders-vulkan/" + path, defines);
}

Shader* Shader::loadEngineShaderNames(std::string path, std::vector<std::string> fileNames, std::vector<std::string> defines) {
	if (! BaseEngine::usingVulkan())
		return loadShaderNames("resources/shaders/" + path, fileNames, defines);
	else
		return loadShaderNames("resources/shaders-vulkan/" + path, fileNames, defines);
}

void Shader::outputCompleteShaderFile(std::string inputPath, std::string outputPath, unsigned int uboBindingOffset, std::string preSource) {
	//Load the source
	ShaderSource shaderSource = loadShaderSource(inputPath, uboBindingOffset, preSource);

	//Write the shader source
	utils_file::writeFile(outputPath, shaderSource.source);
}

void Shader::outputCompleteShaderFiles(std::string inputPath, std::string outputPath, unsigned int uboBindingOffset, std::string preSource) {
	//Load each individual shader if found
	if (utils_file::isFile(inputPath + ".vs"))
		//Output the vertex shader
		outputCompleteShaderFile(inputPath + ".vs", outputPath + "_complete.vert", uboBindingOffset, preSource);
	if (utils_file::isFile(inputPath + ".gs"))
		//Output the geometry shader
		outputCompleteShaderFile(inputPath + ".gs", outputPath + "_complete.geom", uboBindingOffset, preSource);
	if (utils_file::isFile(inputPath + ".fs"))
		//Output the fragment shader
		outputCompleteShaderFile(inputPath + ".fs", outputPath + "_complete.frag", uboBindingOffset, preSource);
}

void Shader::outputCompleteShaderFiles(std::string inputPath, std::string outputPath, std::vector<std::string> fileNames, unsigned int uboBindingOffset, std::string preSource) {
	for (std::string fileName : fileNames) {
		//Split by extension
		std::vector<std::string> split = utils_string::strSplitLast(fileName, ".");
		//Output file e.g. raygen.rgen -> raygen_complete.rgen
		outputCompleteShaderFile(inputPath + fileName, outputPath + split[0] + "_complete." + split[1], uboBindingOffset, preSource);
	}
}

void Shader::compileToSPIRV(std::string inputPath, std::string outputPath, std::string glslangValidatorPath, std::vector<std::string> defines) {
	//The extra code to add on at the start
	std::string preSource = "";

	for (std::string& define : defines)
		preSource += "#define " + define + "\n";

	//Output the complete shader file
	outputCompleteShaderFiles(inputPath, outputPath, ShaderBuffer::VULKAN_BINDING_OFFSET, preSource);

	//Now compile each shader (if they exist)
	if (utils_file::isFile(outputPath + "_complete.vert"))
		std::system((glslangValidatorPath + " -V " + outputPath + "_complete.vert -o " + outputPath + "_vert.spv").c_str());
	if (utils_file::isFile(outputPath + "_complete.geom"))
		std::system((glslangValidatorPath + " -V " + outputPath + "_complete.geom -o " + outputPath + "_geom.spv").c_str());
	if (utils_file::isFile(outputPath + "_complete.frag"))
		std::system((glslangValidatorPath + " -V " + outputPath + "_complete.frag -o " + outputPath + "_frag.spv").c_str());
}

void Shader::compileToSPIRV(std::string inputPath, std::string outputPath, std::vector<std::string> fileNames, std::string glslangValidatorPath, std::vector<std::string> defines) {
	//The extra code to add on at the start
	std::string preSource = "";

	for (std::string& define : defines)
		preSource += "#define " + define + "\n";

	//Output the complete shader file
	outputCompleteShaderFiles(inputPath, outputPath, fileNames, ShaderBuffer::VULKAN_BINDING_OFFSET, preSource);

	//Now compile each shader
	for (std::string fileName : fileNames) {
		//Split by extension
		std::vector<std::string> split = utils_string::strSplitLast(fileName, ".");

		std::cout << glslangValidatorPath + " --target-env vulkan1.2 -V " + outputPath + split[0] + "_complete." + split[1] + " -o " + outputPath + fileName + ".spv" << std::endl;

		//Output file e.g. raygen_complete.rgen -> raygen.rgen.spv
		std::system((glslangValidatorPath + " --target-env vulkan1.2 -V " + outputPath + split[0] + "_complete." + split[1] + " -o " + outputPath + fileName + ".spv").c_str());
	}
}

void Shader::compileEngineShaderToSPIRV(std::string path, std::string glslangValidatorPath, std::vector<std::string> defines) {
	//Extra part of file name (for Vulkan)
	std::string extraName = "";
	if (defines.size() > 0)
		extraName += "_" + utils_string::strJoin(defines, "");

	//Add the prefix's to get the input and output paths
	compileToSPIRV("resources/shaders/" + path, "resources/shaders-vulkan/" + path + extraName, glslangValidatorPath, defines);
}

void Shader::compileEngineShaderToSPIRV(std::string path, std::vector<std::string> fileNames, std::string glslangValidatorPath, std::vector<std::string> defines) {
	//Add the prefix's to get the input and output paths
	compileToSPIRV("resources/shaders/" + path, "resources/shaders-vulkan/" + path, fileNames, glslangValidatorPath, defines);
}