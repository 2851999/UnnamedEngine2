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

#include "../core/BaseEngine.h"
#include "../core/render/Camera.h"
#include "../core/render/Renderer.h"
#include "../core/ResourceLoader.h"
#include "../core/gui/Font.h"
#include "../core/gui/GUIButton.h"
#include "../core/gui/GUILoadingBar.h"
#include "../core/gui/GUICheckBox.h"
#include "../core/gui/GUIDropDownMenu.h"
#include "../core/gui/GUIDropDownList.h"
#include "../core/gui/GUIPanel.h"
#include "../core/gui/GUILabel.h"
#include "../core/gui/GUISlider.h"
#include "../core/gui/GUITextBox.h"
#include "../core/gui/GUIGroup.h"
#include "../core/gui/GUIRadioCheckBox.h"
#include "../core/ml/ML.h"

class Test : public BaseEngine {
private:
	Camera2D* camera;
	GUIPanel* panel;
	GUIButton* button;
	GUILoadingBar* loadingBar;
	GUICheckBox* checkBox;
	GUIDropDownMenu* dropDownMenu;
	GUIDropDownList* dropDownList;
	GUILabel* label;
	GUISlider* verticalSlider;
	GUISlider* horizontalSlider;
	GUITextBox* textBox;
	GUIGroup* radioCheckBoxGroup;
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
//	getSettings().videoVSync = false;
//	getSettings().videoMaxFPS = 0;
}

void Test::created() {
	camera = new Camera2D(Matrix4f().initOrthographic(0, getSettings().windowWidth, getSettings().windowHeight, 0, -1, 1));
	camera->update();

	GUIComponentRenderer::DEFAULT_FONT = new Font("resources/fonts/ARIAL.TTF", 22, Colour::WHITE, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST));

	panel = new GUIPanel();

	std::vector<Colour> colours = { Colour::RED, Colour::GREEN, Colour::BLUE };

	button = new GUIButton("Hello World!", 200, 20, colours);
	button->setPosition(400, 200);
	button->setBorder(new GUIBorder(button, 1, Colour::ORANGE));

	loadingBar = new GUILoadingBar(200, 20, 10);
	loadingBar->setPosition(400, 300);
	loadingBar->setBorder(new GUIBorder(loadingBar, 1, Colour::ORANGE));
	loadingBar->completedStage();
	loadingBar->completedStage();
	loadingBar->completedStage();

	checkBox = new GUICheckBox("Check Box", 20, 20, colours);
	checkBox->setPosition(400, 400);
	checkBox->setBorder(new GUIBorder(checkBox, 2, Colour::ORANGE));

	GUIButton* dropDownMenuButton = new GUIButton("File", 200, 20, colours);
	dropDownMenu = new GUIDropDownMenu(dropDownMenuButton);
	dropDownMenu->addButton(new GUIButton("Save", 200, 20, colours));
	dropDownMenu->addButton(new GUIButton("Save As", 200, 20, colours));
	dropDownMenu->setPosition(400, 20);

	GUIButton* dropDownListButton = new GUIButton("Select", 200, 20, colours);
	dropDownList = new GUIDropDownList(dropDownListButton, ResourceLoader::sLoadTexture("C:/UnnamedEngine/textures/DropDownOverlayClosed.png"), ResourceLoader::sLoadTexture("C:/UnnamedEngine/textures/DropDownOverlayOpened.png"));
	dropDownList->addButton(new GUIButton("800 x 600", 200, 20, colours));
	dropDownList->addButton(new GUIButton("1280 x 720", 200, 20, colours));
	dropDownList->addButton(new GUIButton("1920 x 1080", 200, 20, colours));
	dropDownList->setPosition(700, 20);

	label = new GUILabel("Hello World!");
	label->setPosition(600, 600);

	GUIButton* verticalSliderButton = new GUIButton("", 26, 10, colours);
	verticalSlider = new GUISlider(verticalSliderButton, GUISlider::VERTICAL, 4, 110, Colour::WHITE);
	verticalSlider->setPosition(20, 400);
	verticalSlider->setInterval(10.0f);

	GUIButton* horizontalSliderButton = new GUIButton("", 10, 26, colours);
	horizontalSlider = new GUISlider(horizontalSliderButton, GUISlider::HORIZONTAL, 110, 4, Colour::WHITE);
	horizontalSlider->setPosition(100, 400);

	textBox = new GUITextBox(Colour::WHITE, 200, 20);
	textBox->setFont(new Font("resources/fonts/ARIAL.TTF", 22, Colour::BLACK, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST)));
	textBox->setDefaultTextFont(new Font("resources/fonts/ARIAL.TTF", 22, Colour::GREY, TextureParameters().setShouldClamp(true).setFilter(GL_NEAREST)));
	textBox->setPosition(20, 300);
	textBox->setDefaultText("Enter something");
	textBox->setBorder(new GUIBorder(textBox, 1.0f, Colour::LIGHT_BLUE));
	//textBox->borderEnabled = true;
	textBox->selection->setColour(Colour(Colour::LIGHT_BLUE, 0.2f));

	radioCheckBoxGroup = new GUIGroup();
	GUIRadioCheckBox* box0 = new GUIRadioCheckBox("Box 0", 20, 20, colours);
	box0->setPosition(0, 0);
	box0->setBorder(new GUIBorder(box0, 2, Colour::ORANGE));
	GUIRadioCheckBox* box1 = new GUIRadioCheckBox("Box 1", 20, 20, colours);
	box1->setPosition(0, 25);
	box1->setBorder(new GUIBorder(box1, 2, Colour::ORANGE));
	GUIRadioCheckBox* box2 = new GUIRadioCheckBox("Box 2", 20, 20, colours);
	box2->setPosition(0, 50);
	box2->setBorder(new GUIBorder(box2, 2, Colour::ORANGE));
	radioCheckBoxGroup->add(box0);
	radioCheckBoxGroup->add(box1);
	radioCheckBoxGroup->add(box2);
	radioCheckBoxGroup->setPosition(700, 400);
	radioCheckBoxGroup->show(); //Find a way to remove this

	panel->add(button);
	panel->add(loadingBar);
	panel->add(checkBox);
	panel->add(dropDownMenu);
	panel->add(dropDownList);
	panel->add(label);
	panel->add(verticalSlider);
	panel->add(horizontalSlider);
	panel->add(textBox);
	panel->add(radioCheckBoxGroup);

	panel->show();

	//panel->enable();

	getWindow()->centreCursor();
	Renderer::addCamera(camera);

	MLDocument document;
	document.load("C:/Users/Joel/Desktop/Idea.xml");
	//document.save("C:/Users/Joel/Desktop/Test.xml");
}

void Test::update() {
	panel->update();

	//std::cout << verticalSlider->getValue() << std::endl;
}

void Test::render() {
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	panel->render();
}

void Test::destroy() {
	delete camera;
	delete panel;
}

#endif /* BASEENGINETEST2D_H_ */
