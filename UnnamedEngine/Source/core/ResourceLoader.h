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

#ifndef CORE_RESOURCELOADER_H_
#define CORE_RESOURCELOADER_H_

#include "audio/Audio.h"
#include "render/MeshLoader.h"
#include "render/Shader.h"
#include "render/Texture.h"
#include "gui/Font.h"

/*****************************************************************************
 * The ResourceLoader class is used to help make loading resources easier
 * This has static methods to load things, as well as allowing an instance
 * to be created to assign specific paths to the resources
 *****************************************************************************/

class ResourceLoader {
private:
	/* The path of all of the resources */
	std::string path;

	/* The path in addition to above for various things indicated by the
	 * variable name */
	std::string pathTextures;
	std::string pathAudio;
	std::string pathShaders;
	std::string pathModels;
	std::string pathFonts;
public:
	/* The constructors */
	ResourceLoader() {}
	ResourceLoader(std::string path) : path(path) {}

	/* The destructor */
	virtual ~ResourceLoader() {}

	/* Various methods to load resources using the paths assigned */
	inline Texture* loadTexture(std::string fileName) const { return sLoadTexture(getAbsPathTextures() + fileName); }
	inline Texture* loadTexture(std::string fileName, TextureParameters parameters) const { return sLoadTexture(getAbsPathTextures() + fileName, parameters); }
	inline Texture* loadTexture(std::string fileName, TextureParameters parameters, bool applyParameters) const { return sLoadTexture(getAbsPathTextures() + fileName, parameters, applyParameters); }

	inline AudioData* loadAudio(std::string fileName) const { return sLoadAudio(getAbsPathAudio() + fileName); }

	inline Shader* loadShader(std::string fileName) const { return sLoadShader(getAbsPathShaders() + fileName); }

	inline Mesh* loadModel(std::string fileName, bool pbr = false) const { return sLoadModel(getAbsPathModels(), fileName, pbr); }
	inline Mesh* loadModel(std::string folder, std::string fileName, bool pbr = false) const { return sLoadModel(getAbsPathModels() + folder, fileName, pbr); }

	inline Font* loadFont(std::string fileName, int size = 18, Colour colour = Colour::WHITE) const { return sLoadFont(getAbsPathFonts() + fileName, size, colour); }

	/* Setters and getters */
	inline void setPath(std::string path) { this->path = path; }
	inline void setPathTextures(std::string path) { pathTextures = path; }
	inline void setPathAudio(std::string path) { pathAudio = path; }
	inline void setPathShaders(std::string path) { pathShaders = path; }
	inline void setPathModels(std::string path) { pathModels = path; }
	inline void setPathFonts(std::string path) { pathFonts = path; }

	inline std::string getPath() const { return path; }
	inline std::string getPathTextures() const { return pathTextures; }
	inline std::string getPathAudio() const { return pathAudio; }
	inline std::string getPathShaders() const { return pathShaders; }
	inline std::string getPathModels() const { return pathModels; }
	inline std::string getPathFonts() const { return pathFonts; }
	inline std::string getAbsPathTextures() const { return path + pathTextures; }
	inline std::string getAbsPathAudio() const { return path + pathAudio; }
	inline std::string getAbsPathShaders() const { return path + pathShaders; }
	inline std::string getAbsPathModels() const { return path + pathModels; }
	inline std::string getAbsPathFonts() const { return path + pathFonts; }

	/* Various static methods to load resources */
	inline static Texture* sLoadTexture(std::string path) { return Texture::loadTexture(path); }
	inline static Texture* sLoadTexture(std::string path, TextureParameters parameters) { return Texture::loadTexture(path, parameters); }
	inline static Texture* sLoadTexture(std::string path, TextureParameters parameters, bool applyParameters) { return Texture::loadTexture(path, parameters, applyParameters); }

	inline static AudioData* sLoadAudio(std::string path) { return AudioLoader::loadFile(path); }

	inline static Shader* sLoadShader(std::string path) { return Shader::loadShader(path); }

	inline static Mesh* sLoadModel(std::string path, std::string fileName, bool pbr = false) { return MeshLoader::loadModel(path, fileName, pbr); }

	inline static Font* sLoadFont(std::string path, int size = 18, Colour colour = Colour::WHITE) { return new Font(path, size, colour); }
};


#endif /* CORE_RESOURCELOADER_H_ */
