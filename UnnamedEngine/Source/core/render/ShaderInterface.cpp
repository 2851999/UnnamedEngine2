/*****************************************************************************
 *
 *   Copyright 2019 Joel Davies
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

#include "ShaderInterface.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The ShaderInterface class
 *****************************************************************************/

/* The ids for particular shader blocks */
const std::string ShaderInterface::BLOCK_MATERIAL = "Material";
const std::string ShaderInterface::BLOCK_SKINNING = "Skinning";
const std::string ShaderInterface::BLOCK_LIGHTING = "Lighting";

/* Binding locations for shader blocks */
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_MATERIAL = 2;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_SKINNING = 3;
const unsigned int ShaderInterface::UBO_BINDING_LOCATION_LIGHTING = 4;

ShaderInterface::ShaderInterface() {
	//Add all required UBOs for the default shaders
	add(BLOCK_MATERIAL, new UBO(NULL, sizeof(ShaderBlock_Material), GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_MATERIAL));
	add(BLOCK_SKINNING, new UBO(NULL, sizeof(ShaderBlock_Skinning), GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_SKINNING));
	add(BLOCK_LIGHTING, new UBO(NULL, sizeof(ShaderBlock_Lighting), GL_DYNAMIC_DRAW, UBO_BINDING_LOCATION_LIGHTING));
}

ShaderInterface::~ShaderInterface() {
	//Go through and delete all UBO's
	for (auto it : ubos)
		delete it.second;
	ubos.clear();
}

UBO* ShaderInterface::getUBO(std::string id) {
	if (ubos.count(id) > 0)
		return ubos.at(id);
	else {
		Logger::log("The UBO with the id '" + id + "' could not be found", "ShaderInterface", LogType::Error);
		return NULL;
	}
}
