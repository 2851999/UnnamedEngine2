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
	inline Texture* loadTexture(std::string fileName) { return sLoadTexture(path + pathTextures + fileName); }
	inline Texture* loadTexture(std::string fileName, TextureParameters parameters) { return sLoadTexture(path + pathTextures + fileName, parameters); }
	inline Texture* loadTexture(std::string fileName, TextureParameters parameters, bool applyParameters) { return sLoadTexture(path + pathTextures + fileName, parameters, applyParameters); }

	inline AudioData* loadAudio(std::string fileName) { return sLoadAudio(path + pathAudio + fileName); }

	inline Shader* loadShader(std::string fileName) { return sLoadShader(path + pathShaders + fileName); }

	inline std::vector<Mesh*> loadModel(std::string fileName) { return sLoadModel(path + pathModels, fileName); }
	inline std::vector<Mesh*> loadModel(std::string folder, std::string fileName) { return sLoadModel(path + pathModels + folder, fileName); }

	/* Setters and getters */
	inline void setPath(std::string path) { this->path = path; }
	inline void setPathTextures(std::string path) { pathTextures = path; }
	inline void setPathAudio(std::string path) { pathAudio = path; }
	inline void setPathShaders(std::string path) { pathShaders = path; }
	inline void setPathModels(std::string path) { pathModels = path; }

	inline std::string getPath() { return path; }
	inline std::string getPathTextures() { return pathTextures; }
	inline std::string getPathShaders() { return pathShaders; }
	inline std::string getPathModels() { return pathModels; }

	/* Various static methods to load resources */
	inline static Texture* sLoadTexture(std::string path) { return Texture::loadTexture(path); }
	inline static Texture* sLoadTexture(std::string path, TextureParameters parameters) { return Texture::loadTexture(path, parameters); }
	inline static Texture* sLoadTexture(std::string path, TextureParameters parameters, bool applyParameters) { return Texture::loadTexture(path, parameters, applyParameters); }

	inline static AudioData* sLoadAudio(std::string path) { return AudioLoader::loadFile(path); }

	inline static Shader* sLoadShader(std::string path) { return Shader::loadShader(path); }

	inline static std::vector<Mesh*> sLoadModel(std::string path, std::string fileName) { return Model::loadModel(path, fileName); }

	inline static Font* sLoadFont(std::string path, int size = 18, Colour colour = Colour::WHITE) { return new Font(path, size, colour, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST)); }
};


#endif /* CORE_RESOURCELOADER_H_ */
