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

#include "GUILabel.h"

GUILabel::GUILabel(std::string text, Font* font) : GUIComponent() {
	//Assign the text
	setText(text);
	//Assign the font if given
	if (font)
		setFont(font);
	//Assign the size
	setSize(getFont()->getWidth(text), getFont()->getHeight(text));
}

void GUILabel::render(bool overrideShader) {
	GUIComponent::render();
	if (visible)
		renderTextAtCentre(text);
}
