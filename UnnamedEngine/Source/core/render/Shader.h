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

#ifndef CORE_RENDER_SHADER_H_
#define CORE_RENDER_SHADER_H_

#include <GL/glew.h>
#include <map>

#include "Colour.h"
#include "../Matrix.h"
#include "../Resource.h"

/*****************************************************************************
 * The Shader class handles a shader program
 *****************************************************************************/

class Shader : public Resource {
private:
	static Shader* currentShader;

	/* OpenGL shader stuff */
	GLint program = -1;
	GLint vertexShader = -1;
	GLint fragmentShader = -1;

	/* Maps with locations of the variables in the shaders */
	std::map<std::string, GLint> uniforms;
	std::map<std::string, GLint> attributes;
public:
	/* The ShaderSource struct stores information about Shader source code */
	struct ShaderSource {
		std::string source;
		std::map<std::string, std::string> uniforms;
		std::map<std::string, std::string> attributes;
	};

	/* Constructors and destructors */
	Shader() {}
	Shader(GLint vertexShader, GLint fragmentShader);
	virtual ~Shader();

	/* Various shader functions */
	void attach(GLuint shader);
	void detach(GLuint shader);
	void use();
	static void stopUsing();
	virtual void destroy() override;
	void addUniform(std::string id, std::string name);
	void addAttribute(std::string id, std::string name);

	GLint getUniformLocation(std::string id);
	GLint getAttributeLocation(std::string id);

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

	/* Methods to get a reference to the map of uniforms for assigning in
	 * the Renderer */
	std::map<std::string, GLint>& getUniforms() { return uniforms; }

	/* Methods to create a shader */
	static GLint createShader(std::string source, GLenum type);
	static Shader* createShader(ShaderSource vertexSource, ShaderSource fragmentSource);

	/* Methods used to load a shader */
	static ShaderSource loadShaderSource(std::string path);
	static Shader* loadShader(std::string path);
};

/*****************************************************************************
 * The RenderShader class handles a Shader for rendering
 *****************************************************************************/

class RenderShader {
private:
	/* The generic name for this RenderShader */
	std::string name;
	/* The shaders used for forward rendering - will always use the last
	 * shader that was added for rendering to allow them to be overridden */
	std::vector<Shader*> forwardShaders;
public:
	/* Various constructors */
	RenderShader(std::string name, Shader* forwardShader) : name(name) { addForwardShader(forwardShader); }

	/* Methods used to add/remove a forward shader */
	void addForwardShader(Shader* forwardShader);
	void removeForwardShader(Shader* forwardShader);
	void removeLastForwardShader();

	/* The method used to get the shader that should be used */
	Shader* getShader();

	/* Returns the name of the RenderShader */
	std::string getName() { return name; }
};

#endif /* CORE_RENDER_SHADER_H_ */
