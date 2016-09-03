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

#ifndef BASEENGINETEST2D_H_
#define BASEENGINETEST2D_H_

#include "core/BaseEngine.h"
#include "core/render/Camera.h"
#include "core/render/Renderer.h"
#include "core/ResourceLoader.h"
#include "core/gui/Font.h"
#include "core/gui/GUIButton.h"
#include "core/gui/GUILoadingBar.h"
#include "core/gui/GUICheckBox.h"
#include "core/gui/GUIDropDownMenu.h"
#include "core/gui/GUIDropDownList.h"

class Test : public BaseEngine {
private:
	Camera2D* camera;
	GUIButton* button;
	GUILoadingBar* loadingBar;
	GUICheckBox* checkBox;
	GUIDropDownMenu* dropDownMenu;
	GUIDropDownList* dropDownList;
public:
	virtual ~Test() {}

	virtual void initialise() override;
	virtual void created() override;
	virtual void update() override;
	virtual void render() override;
	virtual void destroy() override;

	/* Input methods */
	virtual void onKeyPressed(int key) override {
		if (key == GLFW_KEY_ESCAPE)
			requestClose();
	}
	virtual void onKeyReleased(int key) override {}
	virtual void onChar(int key, char character) override {}

	virtual void onMousePressed(int button) override {}
	virtual void onMouseReleased(int button) override {}
	virtual void onMouseMoved(double x, double y, double dx, double dy) override {}
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override {}

	virtual void onScroll(double dx, double dy) override {}
};

void Test::initialise() {
	getSettings().windowTitle = "Unnamed Engine " + Engine::Version;
}

//template<class... A>
//std::vector<A> toVector(A... args) {
//	std::vector<A> values;
//	return values;
//}

void Test::created() {
	camera = new Camera2D(Matrix4f().initOrthographic(0, getSettings().windowWidth, getSettings().windowHeight, 0, -1, 1));
	camera->update();

	GUIComponentRenderer::DEFAULT_FONT = new Font("resources/fonts/ARIAL.TTF", 22, Colour::WHITE, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST));

	button = new GUIButton("Hello World!", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE });
	button->setPosition(400, 200);
	button->setBorder(new GUIBorder(button, 1, Colour::ORANGE));

	loadingBar = new GUILoadingBar(200, 20, 10);
	loadingBar->setPosition(400, 300);
	loadingBar->setBorder(new GUIBorder(loadingBar, 1, Colour::ORANGE));
	loadingBar->completedStage();
	loadingBar->completedStage();
	loadingBar->completedStage();

	checkBox = new GUICheckBox("Check Box", 20, 20, { Colour::RED, Colour::GREEN, Colour::BLUE });
	checkBox->setPosition(400, 400);
	checkBox->setBorder(new GUIBorder(checkBox, 2, Colour::ORANGE));

	GUIButton* dropDownMenuButton = new GUIButton("File", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE });
	dropDownMenu = new GUIDropDownMenu(dropDownMenuButton);
	dropDownMenu->addButton(new GUIButton("Save", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE }));
	dropDownMenu->addButton(new GUIButton("Save As", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE }));
	dropDownMenu->setPosition(400, 20);

	GUIButton* dropDownListButton = new GUIButton("Select", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE });
	dropDownList = new GUIDropDownList(dropDownListButton, ResourceLoader::sLoadTexture("C:/UnnamedEngine/GUIDropDownOverlayClosed.png"), ResourceLoader::sLoadTexture("C:/UnnamedEngine/GUIDropDownOverlayOpened.png"));
	dropDownList->addButton(new GUIButton("800 x 600", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE }));
	dropDownList->addButton(new GUIButton("1280 x 720", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE }));
	dropDownList->addButton(new GUIButton("1920 x 1080", 200, 20, { Colour::RED, Colour::GREEN, Colour::BLUE }));
	dropDownList->setPosition(700, 20);

	Renderer::addCamera(camera);
}

void Test::update() {
	button->update();
	loadingBar->update();
	checkBox->update();
	dropDownMenu->update();
	dropDownList->update();
}

void Test::render() {
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	button->render();
	loadingBar->render();
	checkBox->render();
	dropDownMenu->render();
	dropDownList->render();
}

void Test::destroy() {
	delete camera;
}

#endif /* BASEENGINETEST2D_H_ */
