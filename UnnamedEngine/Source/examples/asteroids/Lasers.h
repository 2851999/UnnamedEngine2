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

#ifndef EXAMPLES_ASTEROIDS_LASERS_H_
#define EXAMPLES_ASTEROIDS_LASERS_H_

#include "../../core/physics/PhysicsObject.h"
#include "../../core/particles/ParticleSystem.h"
#include "../../core/particles/ParticleEmitter.h"
#include "../../core/particles/ParticleEffect.h"
#include "../../core/audio/SoundSystem.h"

#include "AsteroidGroup.h"
#include "LasersRenderer.h"

class Player;

/*****************************************************************************
 * The Lasers class creates a set of lasers that can be fired and collision
 * tested
 *****************************************************************************/

class Lasers {
private:
	/* Pointer to the sound system */
	SoundSystem* soundSystem;

	/* The maximum number of lasers */
	unsigned int maxLasers;

	/* The renderer used to render the lasers */
	LasersRenderer* renderer;

	/* The physics objects representing the lasers */
	std::vector<PhysicsObject3D*> objects;

	/* Corresponds to the life left of lasers in vector above */
	std::vector<float>            timesLeft;

	/* The index of the next object, that should be used
	 * to determine which physics object to move when
	 * the lasers are fired */
	unsigned int nextIndex;

	/* Particle emitter and particle system for explosion effects */
	SphericalParticleEmitter* particleEmitter;
	ParticleSystem*  particleSystem;

	/* The cooldown time for the lasers (in seconds) */
	double cooldown;

	/* The last time a laser was fired (in seconds) */
	double timeLastLaserFired;
public:
	/* The constructor */
	Lasers(SoundSystem* soundSystem, const ResourceLoader& loader);

	/* The destructor */
	virtual ~Lasers();

	/* Method called to reset the lasers */
	void reset();

	/* The method used to update the lasers */
	void update(float deltaSeconds, AsteroidGroup& closestGroup);

	/* The method used to render the lasers */
	void render();

	/* The method used to fire this set of lasers */
	void fire(Vector3f position, Vector3f rotation, Vector3f front);

	/* Returns whether the lasers can fire */
	inline bool canFire() { return TimeUtils::getSeconds() - timeLastLaserFired > cooldown; }

	std::vector<PhysicsObject3D*>& getObjects() { return objects; }
};

#endif /* EXAMPLES_ASTEROIDS_LASERS_H_ */
