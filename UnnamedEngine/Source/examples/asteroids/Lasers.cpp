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

#include "Lasers.h"
#include "Player.h"

/*****************************************************************************
 * The Lasers class
 *****************************************************************************/

unsigned int Lasers::numInstances = 0;

Lasers::Lasers(AsteroidsGame* game, Ship* ship) : soundSystem(game->getSoundSystem()), ship(ship) {
	//Increment the instance number
	numInstances ++;

	//Assign the instance number
	instanceNumber = numInstances;

	//Add audio source's for this set of lasers
	soundSystem->addSoundEffect("Laser" + StrUtils::str(instanceNumber), game->getResources().getAudioLaser());
	soundSystem->addSoundEffect("Explosion" + StrUtils::str(instanceNumber), game->getResources().getAudioExplosion());

	//Assign the maximum number of lasers
	maxLasers = 20;

	//Setup the lasers renderer
	renderer = new LasersRenderer(game->getResourceLoader(), maxLasers);

	for (unsigned int i = 0; i < maxLasers; i++) {
		GameObject3D* object = new GameObject3D();

		objects.push_back(new PhysicsObject3D(object));
		timesLeft.push_back(0);
		renderer->addLaser(object);
	}

	//Assign the next index (to point to the start of the objects array)
	nextIndex = 0;

	//Setup the particle emitter
	particleEmitter = new SphericalParticleEmitter(1.0f);
	particleEmitter->particleSpawnRate = 120;
	particleEmitter->particleLifeSpan = 2.0f;
	particleEmitter->particleColour = Colour::WHITE;
	particleEmitter->particleSize = 5.0f;

	//Setup the particle system
	particleSystem = new ParticleSystem(particleEmitter, 500);
	//particleSystem->acceleration = Vector3f(0.0f, 5.0f, 0.0f);
	particleSystem->effect = new ParticleEffectColourChange(Colour::WHITE, Colour(0.3f, 0.3f, 0.3f, 0.8f));
	particleSystem->textureAtlas = new TextureAtlas(game->getResourceLoader().loadTexture("ParticleAtlas.png"), 8, 8, 64);

	//Assign the laser cooldown time
	cooldown = 0.5;

	//Assign the last laser fired time
	timeLastLaserFired = 0.0;
}

Lasers::~Lasers() {
	//Destroy all created resources
	delete renderer;
	delete particleSystem;
}

void Lasers::reset() {
	//Reset the renderer
	renderer->hideAll();
	renderer->update();
	//Assign the next index (to point to the start of the objects array)
	nextIndex = 0;
	//Assign the last laser fired time
	timeLastLaserFired = 0.0;
	//Reset the particle system
	particleSystem->reset();
}

void Lasers::update(float deltaSeconds, AsteroidGroup& closestGroup) {
	//Get the closest asteroids
	std::vector<GameObject3D*>& closestAsteroids = closestGroup.getObjects();

	//Go through each laser object and update its physics
	for (unsigned int i = 0; i < objects.size(); i++) {
		//Ensure the laser is visible
		if (renderer->isLaserVisible(i)) {
			//Subtract time from the laser's life
			timesLeft[i] -= deltaSeconds;
			//Check whether the laser should be hidden
			if (timesLeft[i] <= 0)
				renderer->hideLaser(i);
			else
				objects[i]->updatePhysics(deltaSeconds);

			//States whether the laser has hit something
			bool hitSomething = false;

			//Check other objects before asteroids
			hitSomething = ship->checkCollision(objects[i]);

			//Check asteroids if the laser hasn't hit anything yet
			if (! hitSomething) {

				//Go through asteroids in the group
				for (unsigned int j = 0; j < closestAsteroids.size(); j++) {
					//Ensure the current asteroid is visible
					if (closestGroup.isAsteroidVisible(j)) {
						//Get the distance between the current laser object and the current asteroid object
						float distance = (objects[i]->getPosition() - closestAsteroids[j]->getPosition()).length();

						//Get the biggest scale component
						float scale = closestAsteroids[j]->getScale().max();

						//Check for an intersection with the asteroid
						if (distance < 9.21f / 2.0f * scale) {
							//Hide the laser and the asteroid
							renderer->hideLaser(i);
							closestGroup.hideAsteroid(j);
							//Create an explosion
							explode(closestAsteroids[j]->getPosition(), 2.0f * scale);
						}
					}
				}
			} else
				//Hide the laser
				renderer->hideLaser(i);
		}
	}

	//Update the renderer
	renderer->update();

	//Update the particle system
	particleSystem->update(deltaSeconds, ((Camera3D*) Renderer::getCamera())->getPosition());
}

void Lasers::render() {
	//Render the lasers
	renderer->render();
	//Render the particles
	particleSystem->render();
}

void Lasers::explode(Vector3f position, float maxSpeed) {
	//Move the particle emitter
	particleEmitter->setPosition(position);
	//Activate the particle emitter
	particleEmitter->emitParticles(100);
	particleEmitter->particleMaxSpeed = maxSpeed;
	//Play the explosion sound effect at the position of the asteroid
	soundSystem->getSource("Explosion" + StrUtils::str(instanceNumber))->setPosition(position);
	soundSystem->play("Explosion" + StrUtils::str(instanceNumber));
}

void Lasers::fire(Vector3f position, Vector3f rotation, Vector3f front, Vector3f currentVelocity) {
	//Ensure the lasers can fire
	if (canFire()) {
		//Assign the new time the last laser was fired
		timeLastLaserFired = TimeUtils::getSeconds();
		//Assign the object's properties
		objects[nextIndex]->setPosition(position);
		objects[nextIndex]->setVelocity(currentVelocity + (front * 20.0f));
		objects[nextIndex]->setRotation(rotation * Vector3f(0.0f, -1.0f, 0.0f));
		timesLeft[nextIndex] = 3.0f;

		//Play the sound effect
		soundSystem->getSource("Laser" + StrUtils::str(instanceNumber))->setParent(objects[nextIndex]);
		soundSystem->play("Laser" + StrUtils::str(instanceNumber));

		//Make the laser visible
		renderer->showLaser(nextIndex);
		//Increment the index, and then ensure it is within the bounds of the objects array
		nextIndex++;
		if (nextIndex >= objects.size())
			nextIndex = 0;
	}
}
