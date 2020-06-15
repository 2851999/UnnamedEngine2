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

#include "ParticleSystem.h"

#include <algorithm>

#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "../render/Renderer.h"

/*****************************************************************************
 * The ParticleSystem class
 *****************************************************************************/

/* The vertex data required by each particle */
const GLfloat ParticleSystem::vertexBufferData[] = {
	 -0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 -0.5f, 0.5f, 0.0f,
	 0.5f, 0.5f, 0.0f
};

/* Locations for the attributes in the shader */
const unsigned int ParticleSystem::ATTRIBUTE_LOCATION_POSITION_DATA = 7;
const unsigned int ParticleSystem::ATTRIBUTE_LOCATION_COLOUR        = 8;
const unsigned int ParticleSystem::ATTRIBUTE_LOCATION_TEXTURE_DATA  = 9;

ParticleSystem::ParticleSystem(ParticleEmitter* emitter, unsigned int maxParticles) : emitter(emitter), maxParticles(maxParticles) {
	//Assign the emitter's system
	emitter->setParticleSystem(this);
	//Generate the OpenGL VBO's
	std::vector<GLfloat> data;
	for (unsigned int i = 0; i < 12u; ++i)
		data.push_back(vertexBufferData[i]);

	renderData = new RenderData(GL_TRIANGLE_STRIP, 4);

	shader = Renderer::getRenderShader(Renderer::SHADER_PARTICLE_SYSTEM);

	vboVertices = new VBO<float>(GL_ARRAY_BUFFER, sizeof(vertexBufferData), data, VBOUsage::STATIC, false);
	vboVertices->addAttribute(ShaderInterface::ATTRIBUTE_LOCATION_POSITION, 3, 0);
	renderData->addVBO(vboVertices);

	//Assign the data about the particles
	particlePositionSizeData.resize(maxParticles * 4);
	particleColourData.resize(maxParticles * 4);
	particleTextureData.resize(maxParticles * 4);

	vboPositionSizeData = new VBO<float>(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(float), particlePositionSizeData, VBOUsage::STREAM, true);
	vboPositionSizeData->addAttribute(ATTRIBUTE_LOCATION_POSITION_DATA, 4, 1);
	renderData->addVBO(vboPositionSizeData);

	vboColours = new VBO<float>(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(float), particleColourData, VBOUsage::STREAM, true);
	vboColours->addAttribute(ATTRIBUTE_LOCATION_COLOUR, 4, 1);
	renderData->addVBO(vboColours);

	vboTextureData = new VBO<float>(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(float), particleTextureData, VBOUsage::STREAM, true);
	vboTextureData->addAttribute(ATTRIBUTE_LOCATION_TEXTURE_DATA, 4, 1);
	renderData->addVBO(vboTextureData);

	particles.resize(maxParticles);

	renderData->setup(shader);

	//Setup the material
	material = new Material();
	material->setup();

	//Setup the billboard descriptor set
	descriptorSetBillboard = new DescriptorSet(Renderer::getShaderInterface()->getDescriptorSetLayout(ShaderInterface::DESCRIPTOR_SET_DEFAULT_BILLBOARD));
	descriptorSetBillboard->setup();

	//Create the graphics pipeline for rendering
	graphicsPipeline = new GraphicsPipeline(Renderer::getGraphicsPipelineLayout(Renderer::GRAPHICS_PIPELINE_PARTICLE_SYSTEM), Renderer::getDefaultRenderPass());
}

ParticleSystem::~ParticleSystem() {
	if (textureAtlas)
		delete textureAtlas;
	if (effect)
		delete effect;
	if (emitter)
		delete emitter;

	delete graphicsPipeline;
	delete descriptorSetBillboard;
	delete material;
	delete renderData;
	delete vboTextureData;
	delete vboColours;
	delete vboPositionSizeData;
	delete vboVertices;
}

void ParticleSystem::reset() {
	//Go through all of the particles
	for (unsigned int i = 0; i < particles.size(); i++) {
		//Assign their life and camera distance
		particles[i].life = -1.0f;
		particles[i].cameraDistance = -1.0f;
	}
	//Sort the particles
	std::sort(&particles.front(), &particles.back());
	//Reset the emitter
	emitter->reset();
}

void ParticleSystem::update(float delta, Vector3f cameraPosition) {
	//Update the particle emitter
	emitter->update(delta);
	//Reset the particle count
	particleCount = 0;
	//The offset for the indices of the current particle
	unsigned int offset = 0;
	//Go through the maximum number of particles
	for (unsigned int i = 0; i < maxParticles; i++) {
		//Make sure the current particle is alive
		if (particles[i].life >= 0) {
			//Subtract the delta from its life
			particles[i].life -= delta;

			//Check whether the particle has just died
			if (particles[i].life <= 0)
				//Assign the camera distance
				particles[i].cameraDistance = -1.0f;

			if (particles[i].life > 0) {
				if (effect)
					effect->update(particles[i], emitter);

				//Increase the speed of the particle
				particles[i].velocity += acceleration * delta;
				//Update the particle's position
				particles[i].position += particles[i].velocity * delta;
				//Update the distance of the particle from the camera
				particles[i].cameraDistance = (particles[i].position - cameraPosition).length();

				//Calculate the offset
				offset = 4 * particleCount;

				//Assign the current particle's position data
				particlePositionSizeData[offset + 0] = particles[i].position.getX();
				particlePositionSizeData[offset + 1] = particles[i].position.getY();
				particlePositionSizeData[offset + 2] = particles[i].position.getZ();

				//Assign the current particle's size
				particlePositionSizeData[offset + 3] = particles[i].size;

				//Assign the current particle's colour
				particleColourData[offset + 0] = particles[i].colour.getR();
				particleColourData[offset + 1] = particles[i].colour.getG();
				particleColourData[offset + 2] = particles[i].colour.getB();
				particleColourData[offset + 3] = particles[i].colour.getA();

				//Check for a texture
				if (textureAtlas) {
					//Calculates the texture index based on the time it has left
					particles[i].textureIndex = (int) ((((float) textureAtlas->getNumTextures()) / emitter->particleLifeSpan) * (emitter->particleLifeSpan - particles[i].life));
					//Get the texture data
					float top, left, bottom, right;
					textureAtlas->getSides(particles[i].textureIndex, top, left, bottom, right);

					//Assign the texture data
					particleTextureData[offset + 0] = top;
					particleTextureData[offset + 1] = left;

					particleTextureData[offset + 2] = bottom;
					particleTextureData[offset + 3] = right;
				}

				//Increment the active particle count
				particleCount++;
			}
		}
	}
	//Sort the particles
	std::sort(particles.begin(), particles.end());

	//Assign the data for the billboarding

	//Get the camera's view matrix used in getting the 'right' and 'up' vectors
	Matrix4f matrix = Renderer::getCamera()->getViewMatrix();

	//Assign the uniforms for the particle shader
	shaderBillboardData.ue_cameraRight = Vector4f(matrix.get(0, 0), matrix.get(0, 1), matrix.get(0, 2), 0.0f);
	shaderBillboardData.ue_cameraUp = Vector4f(matrix.get(1, 0), matrix.get(1, 1), matrix.get(1, 2), 0.0f);

	shaderBillboardData.ue_projectionViewMatrix = (Renderer::getCamera()->getProjectionViewMatrix());

	//Update the UBO
	descriptorSetBillboard->getUBO(0)->update(&shaderBillboardData, 0, sizeof(ShaderBlock_Billboard));

	//Update the paricle data
	vboPositionSizeData->updateStream(particleCount * sizeof(GLfloat) * 4);
	vboColours->updateStream(particleCount * sizeof(GLfloat) * 4);
	vboTextureData->updateStream(particleCount * sizeof(GLfloat) * 4);
}

void ParticleSystem::render() {
	if (particleCount > 0) {
		//Use the graphics pipeline
		graphicsPipeline->bind();

		//Bind the billboard descriptor set
		descriptorSetBillboard->bind();

		//Draw the instances of the particles
		renderData->setNumInstances(particleCount);

		//Render
		Matrix4f matrix = Matrix4f().initIdentity();
		Renderer::render(renderData, matrix, material);
	}
}

unsigned int ParticleSystem::findUnusedParticle() {
	//Checks the particles starting from where it left off
	for (unsigned int i = lastParticleChecked; i < maxParticles; i++) {
		//Checks to see whether the particle is unused
		if (particles[i].life <= 0) {
			//Used for the next time round - this makes it very likely that only one iteration will be needed
			//to find an unused particle
			lastParticleChecked = i;
			return i;
		}
	}

	//Start searching the rest of the particles starting from the beginning
	for (unsigned int i = 0; i < lastParticleChecked; i++) {
		if (particles[i].life <= 0) {
			lastParticleChecked = i;
			return i;
		}
	}
	//Returns 0 to overwrite the first particle if no
	//unused ones were found
	return 0;
}

Particle& ParticleSystem::getParticle(unsigned int index) {
	return particles[index];
}

void ParticleSystem::setTextureAtlas(TextureAtlas* textureAtlas) {
	this->textureAtlas = textureAtlas;
	material->setDiffuse(textureAtlas->getTexture());
	material->update();
}
