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

#ifndef CORE_GUI_GUICOMPONENT_H_
#define CORE_GUI_GUICOMPONENT_H_

#include "../Object.h"
#include "../render/Colour.h"
#include "../render/Texture.h"
#include "../render/Renderer.h"
#include "../input/Input.h"
#include "../Window.h"

#include "Font.h"

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

	/* Method used to setup the renderer when it is created */
	void setup();
protected:
	/* The number representing the data that should be used from the colours/textures */
	unsigned int renderIndex = 0;
public:
	/* The default font used when creating component renderer's */
	static Font* DEFAULT_FONT ;

	/* The constructors */
	GUIComponentRenderer() {}
	GUIComponentRenderer(float width, float height) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, MESH_DATA_FLAGS)), Renderer::getRenderShader("Material"), width, height) { setup(); }
	GUIComponentRenderer(float width, float height, std::vector<Colour> colours) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, MESH_DATA_FLAGS)), Renderer::getRenderShader("Material"), width, height), colours(colours) { setup(); }
	GUIComponentRenderer(float width, float height, std::vector<Texture*> textures) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, textures.at(0), MESH_DATA_FLAGS)), Renderer::getRenderShader("Material"), width, height), textures(textures) { setup(); }
	GUIComponentRenderer(float width, float height, std::vector<Colour> colours, std::vector<Texture*> textures) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, textures.at(0), MESH_DATA_FLAGS)), Renderer::getRenderShader("Material"), width, height), colours(colours), textures(textures) { setup(); }

	/* Destructor */
	virtual ~GUIComponentRenderer() {}

	/* The method used to update this component ready for rendering */
	virtual void update() override;

	/* Methods to render text at a location relative to the component */
	void renderText(std::string text, Vector2f relPos);
	void renderTextAtCentre(std::string text);

	/* Method used to get the max render index */
	unsigned int getMaxRenderIndex();

	/* Setters and getters */
	inline void setFont(Font* font) { this->font = font; }
	inline bool hasFont() { return font; }
	inline Font* getFont() { return font; }

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
	GUIFill(float width, float height, Colour colour) : GUIComponentRenderer(width, height, { colour }) {}
	GUIFill(float width, float height, Texture* texture) : GUIComponentRenderer(width, height, { texture }) {}
	GUIFill(float width, float height, Colour colour, Texture* texture) : GUIComponentRenderer(width, height, { colour }, { texture }) {}
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

class GUIComponent : public GUIComponentRenderer, InputListener {
private:
	/* The component listeners */
	std::vector<GUIComponentListener*> listeners;
protected:
	/* The name of this component */
	std::string name;

	/* The text of this component */
	std::string text;

	/* Various states of this component */
	bool active       = true;
	bool visible      = true;
	bool mouseHover   = false;
	bool mouseClicked = false;

	/* The border of this component (Can be NULL) */
	GUIBorder* border = NULL;

	/* Method called when this component changes state */
	virtual void onChangeState() {}

	/* Methods used to call component listener events */
	inline void callOnComponentClicked(GUIComponent* component) {
		for (unsigned int i = 0; i < listeners.size(); i++)
			listeners[i]->onComponentClicked(component);
	}
public:
	/* Various constructors */
	GUIComponent() { Window::getCurrentInstance()->getInputManager()->addListener(this); }
	GUIComponent(float width, float height) : GUIComponentRenderer(width, height) { Window::getCurrentInstance()->getInputManager()->addListener(this); }
	GUIComponent(float width, float height, std::vector<Colour> colours) : GUIComponentRenderer(width, height, colours) { Window::getCurrentInstance()->getInputManager()->addListener(this); }
	GUIComponent(float width, float height, std::vector<Texture*> textures) : GUIComponentRenderer(width, height, textures) { Window::getCurrentInstance()->getInputManager()->addListener(this); }
	GUIComponent(float width, float height, std::vector<Colour> colours, std::vector<Texture*> textures) : GUIComponentRenderer(width, height, colours, textures) { Window::getCurrentInstance()->getInputManager()->addListener(this); }

	/* Destructor */
	virtual ~GUIComponent() {
		if (border)
			delete border;
		listeners.clear();
	}

	/* The methods used to add and remove a component listener */
	inline void addListener(GUIComponentListener* listener) {
		listeners.push_back(listener);
	}

	inline void removeListener(GUIComponentListener* listener) {
		listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
	}

	/* The method used to update this component */
	virtual void update() override;

	/* The method used to render this component */
	virtual void render(bool overrideShader = false) override;

	/* The input methods */
	virtual void onKeyPressed(int key) override {}
	virtual void onKeyReleased(int key) override {}
	virtual void onChar(int key, char character) override {}

	virtual void onMousePressed(int button) override;
	virtual void onMouseReleased(int button) override;
	virtual void onMouseMoved(double x, double y, double dx, double dy) override;
	virtual void onMouseDragged(double x, double y, double dx, double dy) override {}
	virtual void onMouseEnter() override {}
	virtual void onMouseLeave() override;

	virtual void onScroll(double dx, double dy) override {}

	/* Setters and getters */
	inline void setName(std::string name) { this->name = name;       }
	inline void setText(std::string text) { this->text = text;       }
	inline void setActive(bool active)    { this->active = active;   }
	inline void setVisible(bool visible)  { this->visible = visible; }
	inline void setBorder(GUIBorder* border) { this->border = border; }

	inline std::string getName() { return name;    }
	inline std::string getText() { return text;    }
	inline bool isActive()       { return active;  }
	inline bool isVisible()      { return visible; }
	inline GUIBorder* getBorder() { return border; }
	inline bool isClicked() { return mouseClicked; }
};

#endif /* CORE_GUI_GUICOMPONENT_H_ */
