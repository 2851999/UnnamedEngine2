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

#pragma once

#include "../Window.h"
#include <unordered_map>

#include "Colour.h"
#include "../Matrix.h"
#include "../Resource.h"

/*****************************************************************************
 * The Shader class handles a shader program
 *****************************************************************************/

class VulkanRenderShader;

class Shader : public Resource {
private:
	static Shader* currentShader;

	/* The shader program */
	GLint program = -1;

	/* The attached shaders */
	std::vector<GLuint> attachedShaders;

	/* Maps with locations of the variables in the shaders */
	std::unordered_map<std::string, GLint> uniforms;
	std::unordered_map<std::string, GLint> attributes;

	/* The shader modules for Vulkan */
	VkShaderModule vertexShaderModule   = VK_NULL_HANDLE;
	VkShaderModule fragmentShaderModule = VK_NULL_HANDLE;

	/* Loads and returns an included file */
	static std::vector<std::string> loadInclude(std::string path, std::string line);

	/* Method to read a file (used for Vulkan shaders) */
	static std::vector<char> readFile(const std::string& path);
public:
	/* The ShaderSource struct stores information about Shader source code */
	struct ShaderSource {
		std::string source;
		std::unordered_map<std::string, std::string> uniforms;
		std::unordered_map<std::string, std::string> attributes;
	};

	/* Constructors and destructors */
	Shader() {}
	Shader(GLint vertexShader, GLint fragmentShader) : Shader(vertexShader, -1, fragmentShader) {}
	Shader(GLint vertexShader, GLint geometryShader, GLint fragmentShader);
	Shader(VkShaderModule vertexShaderModule, VkShaderModule fragmentShaderModule);
	virtual ~Shader();

	/* Various shader functions */
	void attach(GLuint shader);
	void detach(GLuint shader);
	void use();
	void stopUsing();
	virtual void destroy() override;
	void addUniform(std::string id, std::string name);
	void addAttribute(std::string id, std::string name);

	inline GLint getProgram() { return program; }
	GLint getUniformLocation(std::string id);
	GLint getAttributeLocation(std::string id);

	VkShaderModule& getVkVertexShaderModule() { return vertexShaderModule; }
	VkShaderModule& getVkFragmentShaderModule() { return fragmentShaderModule; }

	/* Various methods to assign values */
	void setUniformi(std::string id, GLuint value);
	void setUniformf(std::string id, GLfloat value);
	void setUniformVector2(std::string id, Vector2f value);
	void setUniformVector3(std::string id, Vector3f value);
	void setUniformVector4(std::string id, Vector4f value);
	void setUniformColourRGB(std::string id, Colour colour);
	void setUniformColourRGBA(std::string id, Colour colour);
	void setUniformMatrix3(std::string id, Matrix3f matrix);
	void setUniformMatrix4(std::string id, Matrix4f matrix);

	/* Method to get a reference to the map of uniforms for assigning in
	 * the Renderer */
	std::unordered_map<std::string, GLint>& getUniforms() { return uniforms; }

	/* Methods to create a shader */
	static GLint createShader(std::string source, GLenum type);
	static Shader* createShader(ShaderSource vertexSource, ShaderSource fragmentSource);
	static Shader* createShader(ShaderSource vertexSource, ShaderSource geometrySource, ShaderSource fragmentSource);
	static VkShaderModule createVkShaderModule(const std::vector<char>& code);

	/* Methods used to load a shader */
	static void loadShaderSource(std::string path, std::vector<std::string> &fileText, ShaderSource &source, unsigned int uboBindingOffset = 0);
	static ShaderSource loadShaderSource(std::string path, unsigned int uboBindingOffset = 0);
	static Shader* loadShader(std::string path);

	/* Methods to read a shader and output all of the complete files for that shader (with all includes replaced as requested) - these
	 * will not include mapped uniforms since they are intended for compilation to SPIR-V */
	static void outputCompleteShaderFile(std::string inputPath, std::string outputPath, unsigned int uboBindingOffset = 0, std::string preSource = "");
	static void outputCompleteShaderFiles(std::string inputPath, std::string outputPath, unsigned int uboBindingOffset = 0, std::string preSource = "");

	/* Utility method to use given glslValidator.exe path to compile a shader from the engine to SPIR-V */
	static void compileToSPIRV(std::string inputPath, std::string outputPath, std::string glslangValidatorPath);

	/* Utility method to use the above method to compile an engine shader and place it in the appropriate location */
	static void compileEngineShaderToSPIRV(std::string path, std::string glslangValidatorPath);
};
