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

#include "GUICheckBox.h"

/*****************************************************************************
 * The GUICheckBox class
 *****************************************************************************/

void GUICheckBox::updateRenderIndex() {
	unsigned int maxRenderIndex = getMaxRenderIndex();
	if (checked) {
		if (maxRenderIndex == 1)
			renderIndex = 0;
		else if (maxRenderIndex == 2)
			renderIndex = 1;
		else if (maxRenderIndex == 3)
			renderIndex = 2;
	} else if (mouseHover) {
		if (maxRenderIndex == 1)
			renderIndex = 0;
		else if (maxRenderIndex == 2)
			renderIndex = 1;
		else if (maxRenderIndex == 3)
			renderIndex = 1;
	} else {
		if (maxRenderIndex == 1)
			renderIndex = 0;
		else if (maxRenderIndex == 2)
			renderIndex = 0;
		else if (maxRenderIndex == 3)
			renderIndex = 0;
	}
}

void GUICheckBox::onChangeState() {
	if (mouseClicked)
		checked = ! checked;
	updateRenderIndex();
}

void GUICheckBox::onComponentRender() {
	//Render the text of this checkbox to the left of it
	renderText(text, Vector2f(-getFont()->getWidth(text) * 1.1f, getHeight() / 2 + getFont()->getHeight(text) / 2));
}
