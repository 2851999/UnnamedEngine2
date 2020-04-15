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

#include "../../core/gui/GUILoadingBar.h"
#include "../../core/gui/GUIPanel.h"
#include "../../core/gui/GUILabel.h"

class AsteroidsGame;
class Player;

/*****************************************************************************
 * The HUD class sets up and manages the player HUD
 *****************************************************************************/

class HUD : public GUIPanel {
private:
	/* The game instance */
	AsteroidsGame* game;

	/* The label displaying the player's score */
	GUILabel* labelScore;

	/* Loading bar for the player's health */
	GUILoadingBar* barPlayerHealth;

	/* Loading bar for the player's shield */
	GUILoadingBar* barPlayerShield;

	/* The cross hair object */
	GameObject2D* crossHair;

	/* The player instance */
	Player* player;
public:
	/* The constructor */
	HUD(AsteroidsGame* game, Player* player);

	/* The destructor */
	virtual ~HUD();

	/* Method used to update the HUD */
	virtual void update() override;

	/* Method used to render the HUD*/
	virtual void render() override;
};

