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

Lasers::Lasers(ResourceLoader& loader) {
	//Assign the maximum number of lasers
	maxLasers = 20;

	//Setup the lasers renderer
	renderer = new LasersRenderer(loader, maxLasers);

	for (unsigned int i = 0; i < maxLasers; i++) {
		GameObject3D* object = new GameObject3D();

		objects.push_back(new PhysicsObject3D(object));
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
	particleSystem->textureAtlas = new TextureAtlas(loader.loadTexture("ParticleAtlas.png"), 8, 8, 64);

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

void Lasers::update(float deltaSeconds, AsteroidGroup& closestGroup) {
	//Go through each laser object and update its physics
	for (unsigned int i = 0; i < objects.size(); i++) {
		//Ensure the laser is visible
		if (renderer->isLaserVisible(i))
			objects[i]->updatePhysics(deltaSeconds);
	}

	//Update the renderer
	renderer->update();

	//Get the closest asteroids
	std::vector<GameObject3D*>& closestAsteroids = closestGroup.getObjects();
	//Go through asteroids in the group
	for (unsigned int i = 0; i < closestAsteroids.size(); i++) {
		//Ensure the current asteroid is visible
		if (closestGroup.isAsteroidVisible(i)) {
			//Go through the laser objects
			for (unsigned int j = 0; j < objects.size(); j++) {
				//Ensure the current laser is also visible
				if (renderer->isLaserVisible(j)) {
					//Get the distance between the current laser object and the current asteroid object
					float distance = (objects[j]->getPosition() - closestAsteroids[i]->getPosition()).length();

					float scale = closestAsteroids[i]->getScale().getX();
					if (closestAsteroids[i]->getScale().getY() > scale)
						scale = closestAsteroids[i]->getScale().getY();
					else if (closestAsteroids[i]->getScale().getZ() > scale)
						scale = closestAsteroids[i]->getScale().getZ();

					if (distance < 9.21f * scale) {
						//Hide the laser and the asteroid
						renderer->hideLaser(j);
						closestGroup.hideAsteroid(i);
						//Move the particle emitter
						particleEmitter->setPosition(closestAsteroids[i]->getPosition());
						//Activate the particle emitter
						particleEmitter->emitParticles(100);
						particleEmitter->particleMaxSpeed = 2.0f * scale;
					}
				}
			}
		}
	}

	//Update the particle system
	particleSystem->update(deltaSeconds, ((Camera3D*) Renderer::getCamera())->getPosition());
}

void Lasers::render() {
	//Render the lasers
	renderer->render();
	//Render the particles
	particleSystem->render();
}

void Lasers::fire(Player* player) {
	//Ensure the lasers can fire
	if (canFire()) {
		//Assign the new time the last laser was fired
		timeLastLaserFired = TimeUtils::getSeconds();
		//Assign the object's properties
		objects[nextIndex]->setPosition(player->getCamera()->getPosition());
		objects[nextIndex]->setVelocity(player->getCamera()->getFront() * 10.0f);
		objects[nextIndex]->setRotation(player->getCamera()->getRotation() * Vector3f(0.0f, -1.0f, 0.0f));
		//Make the laser visible
		renderer->showLaser(nextIndex);
		//Increment the index, and then ensure it is within the bounds of the objects array
		nextIndex++;
		if (nextIndex >= objects.size())
			nextIndex = 0;
	}
}
