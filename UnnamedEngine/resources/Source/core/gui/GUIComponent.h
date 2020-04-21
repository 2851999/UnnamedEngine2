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

#include "Text.h"

#include "../Object.h"
#include "../input/Input.h"
#include "../render/Colour.h"
#include "../render/Texture.h"
#include "../render/Renderer.h"

/*****************************************************************************
 * The GUIComponentRenderer class is used to render a GUIComponent
 *****************************************************************************/

class GUIComponentRenderer : public GameObject2D {
private:
	/* The MeshData flags used for the component renderer */
	static const MeshData::Flag MESH_DATA_FLAGS = MeshData::SEPARATE_POSITIONS | MeshData::SEPARATE_TEXTURE_COORDS;

	/* The colours/textures used */
	std::vector<Colour>   colours;
	std::vector<Texture*> textures;

	/* The font used to render text */
	Font* font = DEFAULT_FONT;

	/* The text instance used to render text */
	Text* textInstance = NULL;

	/* Method used to setup the renderer when it is created */
	void setup();
protected:
	/* The number representing the data that should be used from the colours/textures */
	unsigned int renderIndex = 0;
public:
	/* The default font used when creating component renderer's */
	static Font* DEFAULT_FONT;

	/* The constructors */
	GUIComponentRenderer() {}
	GUIComponentRenderer(float width, float height) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, MESH_DATA_FLAGS)), Renderer::SHADER_MATERIAL, width, height) { setup(); }
	GUIComponentRenderer(float width, float height, std::vector<Colour> colours) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, MESH_DATA_FLAGS)), Renderer::SHADER_MATERIAL, width, height), colours(colours) { setup(); }
	GUIComponentRenderer(float width, float height, std::vector<Texture*> textures) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, textures.at(0), MESH_DATA_FLAGS)), Renderer::SHADER_MATERIAL, width, height), textures(textures) { setup(); }
	GUIComponentRenderer(float width, float height, std::vector<Colour> colours, std::vector<Texture*> textures) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, textures.at(0), MESH_DATA_FLAGS)), Renderer::SHADER_MATERIAL, width, height), colours(colours), textures(textures) { setup(); }

	/* Destructor */
	virtual ~GUIComponentRenderer() { delete textInstance; }

	/* The method used to update this component ready for rendering */
	virtual void update() override;

	/* Methods to render text at a location relative to the component */
	void renderText(std::string text, Vector2f relPos);
	void renderTextAtCentre(std::string text);

	/* Method used to get the max render index */
	unsigned int getMaxRenderIndex();

	/* Methods used to set the colours/textures */
	void setColour(Colour colour);
	void setTexture(Texture* texture);

	/* Setters and getters */
	void setFont(Font* font);
	inline void setTextColour(Colour colour) { textInstance->setColour(colour); }

	inline bool hasFont() { return font; }
	inline Font* getFont() { return font; }
	inline Text* getTextInstance() { return textInstance; }
	inline Colour getTextColour() { return textInstance->getColour(); }

	inline std::vector<Colour>& getColours() { return colours; }
	inline std::vector<Texture*>& getTextures() { return textures; }
	inline bool hasColours() { return colours.size() > 0; }
	inline bool hasTextures() { return textures.size() > 0; }
};

/*****************************************************************************
 * The GUIFill class is used to create something that can simply be used to
 * draw a solid colour - This is also similar to GUIComponent however it
 * doesn't receive input
 *****************************************************************************/

class GUIFill : public GUIComponentRenderer {
public:
	/* Various constructors */
	GUIFill(float width, float height) : GUIComponentRenderer(width, height) {}
	GUIFill(float width, float height, Colour colour) : GUIComponentRenderer(width, height, std::vector<Colour> { colour }) {}
	GUIFill(float width, float height, Texture* texture) : GUIComponentRenderer(width, height, std::vector<Texture*> { texture }) {}
	GUIFill(float width, float height, Colour colour, Texture* texture) : GUIComponentRenderer(width, height, std::vector<Colour> { colour }, std::vector<Texture*> { texture }) {}
	GUIFill(float width, float height, std::vector<Colour> colours) : GUIComponentRenderer(width, height, colours) {}
	GUIFill(float width, float height, std::vector<Texture*> textures) : GUIComponentRenderer(width, height, textures) {}
	GUIFill(float width, float height, std::vector<Colour> colours, std::vector<Texture*> textures) : GUIComponentRenderer(width, height, colours, textures) {}

	/* Destructor */
	virtual ~GUIFill() {}

	/* These methods are used to change the size of the fill by scaling it based on the original size */
	void updateWidth(float width);
	void updateHeight(float height);
};

/*****************************************************************************
 * The GUIBorder class is used to create a border for a component
 *****************************************************************************/

class GUIComponent;

class GUIBorder : public GUIFill {
private:
	/* The component this is a border for */
	GUIComponent* component;

	/* The thickness of this border */
	float thickness;
public:
	/* Various constructors */
	GUIBorder(GUIComponent* component, float thickness, Colour colour);
	GUIBorder(GUIComponent* component, float thickness, Texture* texture);
	GUIBorder(GUIComponent* component, float thickness, Colour colour, Texture* texture);
	GUIBorder(GUIComponent* component, float thickness, std::vector<Colour> colours);
	GUIBorder(GUIComponent* component, float thickness, std::vector<Texture*> textures);
	GUIBorder(GUIComponent* component, float thickness, std::vector<Colour> colours, std::vector<Texture*> textures);

	/* Destructor */
	virtual ~GUIBorder() {}
};

/*****************************************************************************
 * The GUIComponentListener class is used to receive component events
 *****************************************************************************/

class GUIComponentListener {
public:
	GUIComponentListener() {}
	virtual ~GUIComponentListener() {}

	/* Called when a component is clicked - more specifically when the
	 * component is released after the mouse is pressed. This is so that, in
	 * the case of a button for something such as GUIDropDownMenu, the buttons
	 * will get the change to change to their clicked colour before
	 * disappearing  */
	virtual void onComponentClicked(GUIComponent* component) {}
};

/*****************************************************************************
 * The GUIComponent class is used to create a component for a GUI
 *****************************************************************************/

class GUIGroup;

class GUIComponent : public GUIComponentRenderer, InputListener {
private:
	/* The component listeners */
	std::vector<GUIComponentListener*> listeners;

	/* Components that are attached to this one */
	std::vector<GUIComponent*> attachedComponents;
protected:
	/* The name of this component */
	std::string name;

	/* The text of this component */
	std::string text;

	/* Various states of this component */
	bool active       = true;
	bool visible      = true;
	bool occluded     = false;
	bool mouseHover   = false;
	bool mouseClicked = false;

	/* The border of this component (Can be NULL) */
	GUIBorder* border = NULL;

	/* The group this component belongs to (Can be NULL) */
	GUIGroup* group = NULL;

	/* Method called to update this component (useful when no need to override
	 * the default component update method */
	virtual void onComponentUpdate() {}

	/* Method called to render this component (useful when no need to override
	 * the default component render method */
	virtual void onComponentRender() {}

	/* Method called to notify this component it has been clicked */
	virtual void onComponentClicked() {}

	/* Method called when this component changes state */
	virtual void onChangeState() {}

	/* Methods used to call component listener events */
	inline void callOnComponentClicked(GUIComponent* component) {
		onComponentClicked();
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners[i]->onComponentClicked(component);
	}
public:
	/* Various constructors */
	GUIComponent() : GUIComponentRenderer() {}
	GUIComponent(float width, float height) : GUIComponentRenderer(width, height) {}
	GUIComponent(float width, float height, std::vector<Colour> colours) : GUIComponentRenderer(width, height, colours) {}
	GUIComponent(float width, float height, std::vector<Texture*> textures) : GUIComponentRenderer(width, height, textures) {}
	GUIComponent(float width, float height, std::vector<Colour> colours, std::vector<Texture*> textures) : GUIComponentRenderer(width, height, colours, textures) {}

	/* Destructor */
	virtual ~GUIComponent();

	/* The methods used to add and remove a component listener */
	void addListener(GUIComponentListener* listener);
	void removeListener(GUIComponentListener* listener);

	/* Method used to add a component to this one */
	inline void add(GUIComponent* component) { component->setParent(this); attachedComponents.push_back(component); }

	/* Method used to check whether a position is within the component */
	bool contains(double x, double y);

	/* Methods used to enable/disable this component (Adds/Removes the input listener instance */
	virtual void enable();
	virtual void disable();

	/* The method used to update this component */
	virtual void update() override;

	/* The method used to render this component */
	virtual void render() override;

	/* The input methods */
	virtual void onKeyPressed(int key) override;
	virtual void onKeyReleased(int key) override;
	virtual void onChar(int key, char character) override;

	virtual void onMousePressed(int button) override;
	virtual void onMouseReleased(int button) override;
	virtual void onMouseMoved(double x, double y, double dx, double dy) override;
	virtual void onMouseDragged(double x, double y, double dx, double dy) override;
	virtual void onMouseEnter() override;
	virtual void onMouseLeave() override;

	virtual void onScroll(double dx, double dy) override;

	/* Setters and getters */
	inline void setName(std::string name) { this->name = name;       }
	inline void setText(std::string text) { this->text = text;       }
	void setActive(bool active);
	void setVisible(bool visible);
	void setOccluded(bool occluded);
	inline void setBorder(GUIBorder* border) { this->border = border; }
	inline void setGroup(GUIGroup* group) { this->group = group; }

	inline std::string getName() { return name;    }
	inline std::string getText() { return text;    }
	inline bool isActive()       { return active;  }
	inline bool isVisible()      { return visible; }
	inline bool isOccluded()     { return occluded; }
	inline GUIBorder* getBorder() { return border; }
	inline bool hasBorder() { return border; }
	inline GUIGroup* getGroup() { return group; }
	inline GUIGroup* hasGroup() { return group; }
	inline bool isMouseHovering() { return mouseHover; }
	inline bool isClicked() { return mouseClicked; }
};

