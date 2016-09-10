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

#ifndef EXAMPLES_ASTEROIDS_PLAYER_H_
#define EXAMPLES_ASTEROIDS_PLAYER_H_

#include "../../utils/DebugCamera.h"
#include "../../core/physics/PhysicsObject.h"
#include "AsteroidsGame.h"
#include "Lasers.h"
#include "Ship.h"

/*****************************************************************************
 * The Player class handles player movement and shooting
 *****************************************************************************/

class Player : public Ship, public InputListener {
private:
	/* Input axis/buttons */
	InputBindingAxis* axisForward;
	InputBindingAxis* axisSideways;
	InputBindingButton* buttonShoot;

	/* The player's score */
	unsigned int score;

	/* The current game delta */
	float currentDelta;

	/* The player camera */
	Camera3D* camera;

	/* The game instance */
	AsteroidsGame* game;

	/* Reference to the list of enemies */
	std::vector<Enemy*>& enemies;
public:
	/* The constructor */
	Player(AsteroidsGame* mainGame, std::vector<Enemy*>& enemies);

	/* The destructor */
	virtual ~Player();

	/* Called to reset the player */
	virtual void reset() override;

	/* Method used to update the player and their lasers */
	void update(float deltaSeconds, AsteroidGroup& closestAsteroids) override;

	/* Method to use the player's view and render the lasers */
	void render();

	/* Called when an asteroid has been destroyed by this ship's lasers */
	virtual void onAsteroidDestroyed(GameObject3D* asteroid) override;

	/* Method used to check whether a laser has collided with anything */
	virtual bool checkCollision(PhysicsObject3D* laser) override;

	/* Input methods */
	virtual void onMouseMoved(double x, double y, double dx, double dy) override;

	/* Setters and getters */
	inline void addPoints(unsigned int points) { score += points; }
	inline unsigned int getScore() { return score; }
	inline Camera3D* getCamera() { return camera; }
};



#endif /* EXAMPLES_ASTEROIDS_PLAYER_H_ */
