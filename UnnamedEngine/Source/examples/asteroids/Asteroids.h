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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDS_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDS_H_

#include "../../core/BaseEngine.h"

#include "MainMenu.h"
#include "MainGame.h"

class Asteroids : public BaseEngine {
private:
	MainMenu* mainMenu = NULL;
	MainGame* mainGame = NULL;

	Camera2D* camera2D = NULL;
public:
	enum GameState {
		MAIN_MENU,
		PLAYING
	};

	Asteroids() { currentState = MAIN_MENU; }
	virtual ~Asteroids();

	virtual void initialise() override;
	virtual void created() override;
	virtual void update() override;
	virtual void render() override;
	virtual void destroy() override;

	void startGame();
private:
	GameState currentState;
};

#endif /* EXAMPLES_ASTEROIDS_ASTEROIDS_H_ */
