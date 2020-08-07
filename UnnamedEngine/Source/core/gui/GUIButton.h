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

#include "GUIComponent.h"

/*****************************************************************************
 * The GUIButton class
 *****************************************************************************/

class GUIButton : public GUIComponent {
public:
	/* The constructors */
	GUIButton(std::string text, float width, float height) : GUIComponent(width, height) { setText(text); }
	GUIButton(std::string text, float width, float height, std::vector<Colour> colours) : GUIComponent(width, height, colours) { setText(text); }
	GUIButton(std::string text, float width, float height, std::vector<Texture*> textures) : GUIComponent(width, height, textures) { setText(text); }
	GUIButton(std::string text, float width, float height, std::vector<Colour> colours, std::vector<Texture*> textures) : GUIComponent(width, height, colours, textures) { setText(text); }

	/* The destructor */
	virtual ~GUIButton() {}
protected:
	/* The method called to render this component */
	virtual void onComponentRender() override;

	/* The method called when the component state has changed */
	virtual void onChangeState() override;
};

