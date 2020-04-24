/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "Shader.h"
#include "DescriptorSet.h"

class GraphicsPipelineLayout;

 /*****************************************************************************
  * The RenderShader class handles a Shader for rendering
  *****************************************************************************/

class RenderShader {
private:
	/* The id for this RenderShader */
	unsigned int id;

	/* The shader */
	Shader* shader;

	/* The layouts of the descriptor sets this render shader requires */
	std::unordered_map<unsigned int, DescriptorSetLayout*> descriptorSetLayouts;
public:
	/* Various constructors */
	RenderShader(unsigned int id, Shader* shader);

	/* Descructor */
	virtual ~RenderShader();

	/* Method used to setup this render shader for use */
	void setup();

	/* Method used to add a descriptor set layout to this render shader */
	inline void add(DescriptorSetLayout* layout) { descriptorSetLayouts.insert(std::pair<unsigned int, DescriptorSetLayout*>(layout->getSetNumber(), layout)); }

	/* Method used to get a descriptor set given it's set number */
	inline DescriptorSetLayout* getDescriptorSetLayout(unsigned int setNumber) { return descriptorSetLayouts.at(setNumber); }

	/* Returns whether there is a descriptor set with a given set number */
	inline bool hasDescriptorSetLayout(unsigned int setNumber) { return descriptorSetLayouts.find(setNumber) != descriptorSetLayouts.end(); }

	/* Setters and getters */
	inline unsigned int getID() { return id; }
	inline Shader* getShader() { return shader; }
	inline std::unordered_map<unsigned int, DescriptorSetLayout*>& getDescriptorSetLayouts() { return descriptorSetLayouts; }
};