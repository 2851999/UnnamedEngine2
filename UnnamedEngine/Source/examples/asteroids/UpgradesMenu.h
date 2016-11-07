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

#ifndef EXAMPLES_ASTEROIDS_UPGRADESMENU_H_
#define EXAMPLES_ASTEROIDS_UPGRADESMENU_H_

#include "../../core/render/Renderer.h"
#include "../../core/render/Camera.h"
#include "../../core/gui/Font.h"
#include "../../core/gui/GUIPanel.h"
#include "../../core/gui/GUILabel.h"

/*****************************************************************************
 * The UpgradesMenu class sets up and manages upgrades menu
 *****************************************************************************/

class AsteroidsGame;
class AsteroidsMainGame;

class UpgradesMenu : public GUIPanel {
private:
	/* The instance of the game */
	AsteroidsGame* game;
	AsteroidsMainGame* mainGame;

	/* The background for the menu */
	GameObject2D* background;

	/* The labels */
	GUILabel* labelFireSpeed;
	GUILabel* labelMovementSpeed;

	/* The buttons on the menu */
	GUIButton* buttonUpgradeFireSpeed;
	GUIButton* buttonUpgradeMovementSpeed;
	GUIButton* buttonBack;

	/* Method used to update the menu */
	void updateMenu();
public:
	/* The constructor */
	UpgradesMenu(AsteroidsGame* game, AsteroidsMainGame* mainGame);

	/* The destructor */
	virtual ~UpgradesMenu();

	/* Method called to setup ready to show the menu */
	void show() override;

	/* Method called to remove the camera and hide the menu */
	void hide() override;

	/* Methods used to render the menu */
	void render() override;

	/* Called when a component is clicked */
	void onComponentClicked(GUIComponent* component) override;
};

#endif /* EXAMPLES_ASTEROIDS_UPGRADESMENU_H_ */
