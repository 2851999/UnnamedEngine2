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

#ifndef CORE_PARTICLES_PARTICLESYSTEM_H_
#define CORE_PARTICLES_PARTICLESYSTEM_H_

#include "../render/Colour.h"
#include "../render/RenderData.h"
#include "../render/TextureAtlas.h"
#include "../render/VBO.h"

/*****************************************************************************
 * The ParticleSystem class is used to manage, update and render a set of
 * particles
 *****************************************************************************/

/* This structure stores the data required about a particle */
struct Particle {
	/* The position and velocity of the particle */
	Vector3f position, velocity;

	/* The colour of the particle */
	Colour colour;

	/* The size of the particle */
	float size;

	/* The time the particle is updated for (in seconds) */
	float life;

	/* The distance to the camera of the particle */
	float cameraDistance;

	/* The texture index if this is animated */
	unsigned int textureIndex;

	/* Used when sorting to place the particles closer to the camera/ones
	 * with -1.0, to the front */
	bool operator<(Particle& other) {
	    return this->cameraDistance < other.cameraDistance;
	}
};

class ParticleEmitter;
class ParticleEffect;

class ParticleSystem {
private:
	/* The vertex data required by each particle */
	static const GLfloat vertexBufferData[];

	/* The data passed to OpenGL about the particles */
	std::vector<GLfloat> particlePositionSizeData; //This stores the position's and size of the particles one after the other
	std::vector<GLfloat> particleColourData;       //This stores the colours of the particles
	std::vector<GLfloat> particleTextureData;      //This stores the data for the texture coordinates for the particles

	/* The render data used for rendering */
	RenderData* renderData;

	/* The VBO's for OpenGL */
	VBO<GLfloat>* vboVertices;
	VBO<GLfloat>* vboPositionSizeData;
	VBO<GLfloat>* vboColours;
	VBO<GLfloat>* vboTextureData;

	/* The shader used for rendering */
	Shader* shader;

	/* A list of the particle structures used to store the data */
	std::vector<Particle> particles;

	/* The last particle that was checked to find an unused particle */
	unsigned int lastParticleChecked = 0;

	/* The last number of active particles when the system was updated */
	unsigned int particleCount = 0;

	/* The particle emitter */
	ParticleEmitter* emitter = NULL;

	/* The maximum number of particles for this system */
	unsigned int maxParticles = 1800;
public:
	/* The particle effect (Can be NULL) */
	ParticleEffect* effect = NULL;

	/* The texture atlas (Can be NULL) */
	TextureAtlas* textureAtlas = NULL;

	/* The acceleration of particles in this system */
	Vector3f acceleration;

	/* The constructor */
	ParticleSystem(ParticleEmitter* emitter, unsigned int maxParticles = 1800);

	/* The destructor */
	virtual ~ParticleSystem();

	/* The method used to update the system */
	void update(float delta, Vector3f cameraPosition);

	/* The method used to render the system */
	void render();

	/* Finds the index of a particle that is not being used - returns 0 if there are no
	 * free particles */
	unsigned int findUnusedParticle();

	/* Returns a reference to a particle at a given index */
	Particle& getParticle(unsigned int index);
};

#endif /* CORE_PARTICLES_PARTICLESYSTEM_H_ */
