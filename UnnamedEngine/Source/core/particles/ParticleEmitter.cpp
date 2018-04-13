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

#include "ParticleEmitter.h"

/*****************************************************************************
 * The ParticleEmitter class
 *****************************************************************************/

void ParticleEmitter::reset() {
	active = false;
	cumulativeDelta = 0;
	particlesToEmit = 0;
}

void ParticleEmitter::update(float delta) {
	//Check whether this emitter is active
	if (active && system) {
		//Cumulate the delta
		cumulativeDelta += delta;

		//Calculate the number of particles to spawn on this update
		unsigned int newParticles = cumulativeDelta * particleSpawnRate;

		//Reset the cumulative delta if some particles are going to be 'spawned'
		if (newParticles > 0) {
			cumulativeDelta = 0;

			//Check whether a set number of particles is being emitted
			if (particlesToEmit > 0) {
				//Check whether the last particles are being emitted
				if ((unsigned int) particlesToEmit <= newParticles) {
					newParticles = particlesToEmit;
					particlesToEmit = 0;
					active = false;
				} else
					//Subtract from the number of particles left to be emitted
					particlesToEmit -= newParticles;
			}
		}

		//Go through the new particles
		for (unsigned int i = 0; i < newParticles; i++) {
			//Get the index of an unused particle
			unsigned int index = system->findUnusedParticle();

			//Emit the particle
			Particle& particle = system->getParticle(index);
			particle.position = getPosition();
			particle.colour = particleColour;
			//particle.colour = Colour(utils_random::randomFloat(), utils_random::randomFloat(), utils_random::randomFloat(), utils_random::randomFloat());
			particle.size = particleSize;
			particle.life = particleLifeSpan;
			particle.velocity = particleInitialVelocity;
			particle.textureIndex = 0;

			emitParticle(particle);
		}
	}
}

void ParticleEmitter::emitParticles(unsigned int count) {
	//Assign the number of particles to emit
	particlesToEmit = count;
	//Make this emitter active
	active = true;
}

void SphericalParticleEmitter::emitParticle(Particle& particle) {
	float theta = utils_random::randomFloat(0, 2 * utils_maths::PI);
	float phi = utils_random::randomFloat(-utils_maths::PI / 2, utils_maths::PI / 2);
	particle.velocity = Vector3f(particleMaxSpeed * cos(theta) * cos(phi), particleMaxSpeed * sin(phi), particleMaxSpeed * sin(theta) * cos(phi));
}
