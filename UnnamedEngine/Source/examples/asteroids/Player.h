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
#include "LasersRenderer.h"

/*****************************************************************************
 * The Player class handles player movement and shooting
 *****************************************************************************/

class Player : public InputListener {
private:
	/* The player camera */
	DebugCamera* camera;

	/* The game instance */
	AsteroidsGame* game;

	/* The lasers */
	std::vector<PhysicsObject3D*> lasers;

	/* The lasers renderer */
	LasersRenderer* lasersRenderer;

	unsigned int nextLaser;
public:
	/* The constructor */
	Player(AsteroidsGame* mainGame);

	/* The destructor */
	virtual ~Player();

	/* Method used to update the player */
	void update();

	/* Method to use the player's view */
	void useView();

	/* Input methods */
	virtual void onMousePressed(int button) override;

	/* Setters and getters */
	inline DebugCamera* getCamera() { return camera; }
	inline std::vector<PhysicsObject3D*>& getLasers() { return lasers; }
};



#endif /* EXAMPLES_ASTEROIDS_PLAYER_H_ */
