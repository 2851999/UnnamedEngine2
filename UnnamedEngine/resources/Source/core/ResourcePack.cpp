/*****************************************************************************
 *
 *   Copyright 2018 Joel Davies
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

#include "ResourcePack.h"

#include "../utils/Logging.h"

/*****************************************************************************
 * The ResourcePack class
 *****************************************************************************/

void ResourcePack::destroy() {
	for (auto const& it : textureResources)
		delete it.second;
	textureResources.clear();
	for (auto const& it : textureAtlasResources)
		delete it.second;
	textureAtlasResources.clear();
	for (auto const& it : fontResources)
		delete it.second;
	fontResources.clear();
}

Texture* ResourcePack::getTexture(std::string id) {
	Texture* texture = NULL;
	try {
		texture = textureResources.at(id);
	} catch(std::out_of_range& e) {
		Logger::log("Texture with the name '" + id + "' was not found", "ResourcePack", LogType::Error);
	}
	return texture;
}

TextureAtlas* ResourcePack::getTextureAtlas(std::string id) {
	TextureAtlas* textureAtlas = NULL;
	try {
		textureAtlas = textureAtlasResources.at(id);
	} catch(std::out_of_range& e) {
		Logger::log("TextureAtlas with the name '" + id + "' was not found", "ResourcePack", LogType::Error);
	}
	return textureAtlas;
}

Font* ResourcePack::getFont(std::string id) {
	Font* font = NULL;
	try {
		font = fontResources.at(id);
	} catch(std::out_of_range& e) {
		Logger::log("Font with the name '" + id + "' was not found", "ResourcePack", LogType::Error);
	}
	return font;
}
