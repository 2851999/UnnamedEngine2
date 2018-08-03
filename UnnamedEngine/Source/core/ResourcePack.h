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

#ifndef CORE_RESOURCEPACK_H_
#define CORE_RESOURCEPACK_H_

#include <map>

#include "render/TextureAtlas.h"

/*****************************************************************************
 * The ResourcePack class used for loading a group of resources for use
 * in multiple places without loading new instances
 *****************************************************************************/

class ResourcePack {
private:
	/* The resources */
	std::map<std::string, Texture*>      textureResources;
	std::map<std::string, TextureAtlas*> textureAtlasResources;
public:
	/* The constructor */
	ResourcePack() {}

	/* The destructor */
	virtual ~ResourcePack() { destroy(); }

	/* Adds a resource to this resource pack */
	inline void add(std::string id, Texture* resource) { textureResources.insert(std::pair<std::string, Texture*>(id, resource)); }
	inline void add(std::string id, TextureAtlas* resource) { textureAtlasResources.insert(std::pair<std::string, TextureAtlas*>(id, resource)); }

	/* Deletes all added resources */
	void destroy();

	/* Returns the resource with the given id */
	Texture* getTexture(std::string id);
	TextureAtlas* getTextureAtlas(std::string id);
};


#endif /* CORE_RESOURCEPACK_H_ */
