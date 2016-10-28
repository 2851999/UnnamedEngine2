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

ParticleSystem::ParticleSystem(ParticleEmitter* emitter, unsigned int maxParticles) : emitter(emitter), maxParticles(maxParticles) {
	//Generate the OpenGL VBO's
	std::vector<GLfloat> data;
	for (unsigned int i = 0; i < 12u; i++)
		data.push_back(vertexBufferData[i]);

	renderData = new RenderData(GL_TRIANGLE_STRIP, 4);

	shader = Renderer::getRenderShader("Particle")->getShader();

	vboVertices = new VBO<GLfloat>(GL_ARRAY_BUFFER, sizeof(vertexBufferData), data, GL_STREAM_DRAW, true);
	vboVertices->addAttribute(shader->getAttributeLocation("Position"), 3, 0);
	renderData->addVBO(vboVertices);

	//Assign the data about the particles
	for (unsigned int i = 0; i < maxParticles * 4; i++)
		particlePositionSizeData.push_back(0);
	for (unsigned int i = 0; i < maxParticles * 4; i++)
		particleColourData.push_back(0);
	for (unsigned int i = 0; i < maxParticles * 4; i++)
		particleTextureData.push_back(0);

	vboPositionSizeData = new VBO<GLfloat>(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), particlePositionSizeData, GL_STREAM_DRAW, true);
	vboPositionSizeData->addAttribute(shader->getAttributeLocation("PositionsData"), 4, 1);
	renderData->addVBO(vboPositionSizeData);

	vboColours = new VBO<GLfloat>(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), particleColourData, GL_STREAM_DRAW, true);
	vboColours->addAttribute(shader->getAttributeLocation("Colour"), 4, 1);
	renderData->addVBO(vboColours);

	vboTextureData = new VBO<GLfloat>(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), particleTextureData, GL_STREAM_DRAW, true);
	vboTextureData->addAttribute(shader->getAttributeLocation("TextureData"), 4, 1);
	renderData->addVBO(vboTextureData);

	for (unsigned int i = 0; i < maxParticles; i++) {
		particles.push_back(Particle());
		particles[i].colour = Colour::WHITE;
		particles[i].size = 1.0f;
		particles[i].life = -1.0f;
	}

	renderData->setup();
}

ParticleSystem::~ParticleSystem() {
	if (textureAtlas)
		delete textureAtlas;
	if (effect)
		delete effect;
	if (emitter)
		delete emitter;

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
		particles[i].life = -1;
		particles[i].cameraDistance = -1;
	}
	//Sort the particles
	std::sort(&particles.front(), &particles.back());
	//Reset the emitter
	emitter->reset();
}

void ParticleSystem::update(float delta, Vector3f cameraPosition) {
	//Update the particle emitter
	emitter->update(this, delta);
	//Reset the particle count
	particleCount = 0;
	//Go through the maximum number of particles
	for (unsigned int i = 0; i < maxParticles; i++) {

		//Make sure the current particle is alive
		if (particles[i].life >= 0) {
			//Subtract the delta from its life
			particles[i].life -= delta;

			//Check whether the particle has just died
			if (particles[i].life < 0)
				//Assign the camera distance
				particles[i].cameraDistance = -1.0f;

			if (particles[i].life >=  0) {
				if (effect)
					effect->update(particles[i], emitter);

				//Increase the speed of the particle
				particles[i].velocity += acceleration * delta;
				//Update the particle's position
				particles[i].position += particles[i].velocity * delta;
				//Update the distance of the particle from the camera
				particles[i].cameraDistance = (particles[i].position - cameraPosition).length();

				//Assign the current particle's position data
				particlePositionSizeData[4 * particleCount + 0] = particles[i].position.getX();
				particlePositionSizeData[4 * particleCount + 1] = particles[i].position.getY();
				particlePositionSizeData[4 * particleCount + 2] = particles[i].position.getZ();

				//Assign the current particle's size
				particlePositionSizeData[4 * particleCount + 3] = particles[i].size;

				//Assign the current particle's colour
				particleColourData[4 * particleCount + 0] = particles[i].colour.getR();
				particleColourData[4 * particleCount + 1] = particles[i].colour.getG();
				particleColourData[4 * particleCount + 2] = particles[i].colour.getB();
				particleColourData[4 * particleCount + 3] = particles[i].colour.getA();

				//Check for a texture
				if (textureAtlas) {
					//Calculates the texture index based on the time it has left
					particles[i].textureIndex = (int) ((((float) textureAtlas->getNumTextures()) / emitter->particleLifeSpan) * (emitter->particleLifeSpan - particles[i].life));
					//Get the texture data
					float top, left, bottom, right;
					textureAtlas->getSides(particles[i].textureIndex, top, left, bottom, right);

					//Assign the texture data
					particleTextureData[4 * particleCount + 0] = top;
					particleTextureData[4 * particleCount + 1] = left;

					particleTextureData[4 * particleCount + 2] = bottom;
					particleTextureData[4 * particleCount + 3] = right;
				}

				//Increment the active particle count
				particleCount++;
			}
		}
	}
	//Sort the particles
	std::sort(&particles.front(), &particles.back());
}

void ParticleSystem::render() {
	if (particleCount > 0) {
		vboPositionSizeData->updateStream(particleCount * sizeof(GLfloat) * 4);
		vboColours->updateStream(particleCount * sizeof(GLfloat) * 4);
		vboTextureData->updateStream(particleCount * sizeof(GLfloat) * 4);

		//Use the shader
		shader->use();

		//Get the camera's view matrix used in getting the 'right' and 'up' vectors
		Matrix4f matrix = Renderer::getCamera()->getViewMatrix();

		//Assign the uniforms for the particle shader
		shader->setUniformVector3("Camera_Right", Vector3f(matrix.get(0, 0), matrix.get(0, 1), matrix.get(0, 2)));
		shader->setUniformVector3("Camera_Up", Vector3f(matrix.get(1, 0), matrix.get(1, 1), matrix.get(1, 2)));

		shader->setUniformMatrix4("MVPMatrix", (Renderer::getCamera()->getProjectionViewMatrix()));

		if (textureAtlas)
			shader->setUniformi("Texture", Renderer::bindTexture(textureAtlas->getTexture()));
		else
			shader->setUniformi("Texture", Renderer::bindTexture(Renderer::getBlankTexture()));

		//Draw the instances of the particles
		renderData->setNumInstances(particleCount);
		renderData->render();

		Renderer::unbindTexture();

		//Finished with the shader
		shader->stopUsing();
	}
}

unsigned int ParticleSystem::findUnusedParticle() {
	//Checks the particles starting from where it left off
	for (unsigned int i = lastParticleChecked; i < maxParticles; i++) {
		//Checks to see whether the particle is unused
		if (particles[i].life < 0) {
			//Used for the next time round - this makes it very likely that only one iteration will be needed
			//to find an unused particle
			lastParticleChecked = i;
			return i;
		}
	}

	//Start searching the rest of the particles starting from the beginning
	for (unsigned int i = 0; i < lastParticleChecked; i++) {
		if (particles[i].life < 0) {
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
