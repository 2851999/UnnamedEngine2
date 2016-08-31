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

#ifndef EXAMPLES_ASTEROIDS_MAINGAME_H_
#define EXAMPLES_ASTEROIDS_MAINGAME_H_

#include "../../core/render/Scene.h"
#include "../../core/physics/PhysicsScene.h"

class Camera3D;

class MainGame : public InputListener {
private:
	Asteroids* asteroids;
	Camera3D*  camera3D;

	Scene* scene;
	PhysicsObject3D* player;
	PhysicsScene3D* physicsScene;
public:
	/* The constructor */
	MainGame(Asteroids* asteroids, float windowWidth, float windowHeight);

	/* The destructor */
	virtual ~MainGame();

	void startGame();

	/* Update and render methods */
	void update();
	void render();

	virtual void onMouseMoved(double x, double y, double dx, double dy) override;
};



#endif /* EXAMPLES_ASTEROIDS_MAINGAME_H_ */
