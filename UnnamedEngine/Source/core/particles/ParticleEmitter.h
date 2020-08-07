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

#pragma once

#include "ParticleSystem.h"
#include "../Object.h"

/*****************************************************************************
 * The ParticleEmitter class handles the creation of new particles in a
 * ParticleSystem
 *****************************************************************************/

class ParticleEmitter : public GameObject3D {
private:
	/* This is used to so that particles still spawn even if none are on the
	 * current frame due the delta * particleSpawnRate being 0 when cast to an
	 * integer */
	float cumulativeDelta = 0;

	/* Used to emit a set number of particles - assigned using emitParticles()
	 * (<= 0 means the method has not been called */
	int particlesToEmit = 0;

	/* The particle system this emitter is attached to */
	ParticleSystem* system = NULL;
protected:
	/* The overrideable method used to setup and emit a particle */
	virtual void emitParticle(Particle& particle) {}
public:
	/* Determines whether this particle emitter is active */
	bool active = false;

	/* The number of particles to spawn per second */
	unsigned int particleSpawnRate = 100;

	/* The size of the particles emitted - this covers the width/height as
	 * particles are rendered as a square */
	float particleSize = 0.1f;

	/* The life span of a particle measured in seconds */
	float particleLifeSpan = 2.0f;

	/* The initial particle speed */
	Vector3f particleInitialVelocity = Vector3f(0.0f, 1.0f, 0.0f);

	/* The initial particle colour */
	Colour particleColour = Colour::WHITE;

	/* The constructor */
	ParticleEmitter() {}

	/* Method used to reset this emitter */
	void reset();

	/* The method used to update this emitter and emit any required particles */
	void update(float delta, Vector3f cameraPosition);

	/* Method used to emit a set number of particles before becoming no longer active */
	void emitParticles(unsigned int count);

	/* Setters and getters */
	inline void setParticleSystem(ParticleSystem* system) { this->system = system; }
	inline void setActive(bool active) { this->active = active; }
	inline ParticleSystem* getParticleSystem() { return system; }
	inline bool isActive() { return active; }
};

class SphericalParticleEmitter : public ParticleEmitter {
protected:
	/* The overrideable method used to setup and emit a particle */
	virtual void emitParticle(Particle& particle) override;
public:
	/* The max speed */
	float particleMaxSpeed;

	/* The constructor */
	SphericalParticleEmitter(float particleMaxSpeed = 2.0f) : particleMaxSpeed(particleMaxSpeed) {}
};
