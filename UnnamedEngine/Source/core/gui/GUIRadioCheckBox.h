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

#ifndef CORE_GUI_GUIRADIOCHECKBOX_H_
#define CORE_GUI_GUIRADIOCHECKBOX_H_

#include "GUICheckBox.h"

/*****************************************************************************
 * The GUIRadioCheckBox class
 *****************************************************************************/

class GUIRadioCheckBox : public GUICheckBox {
public:
	/* The constructors */
	GUIRadioCheckBox(std::string text, float width, float height) : GUICheckBox(text, width, height) {}
	GUIRadioCheckBox(std::string text, float width, float height, std::vector<Colour> colours) : GUICheckBox(text, width, height, colours) {}
	GUIRadioCheckBox(std::string text, float width, float height, std::vector<Texture*> textures) : GUICheckBox(text, width, height, textures) {}
	GUIRadioCheckBox(std::string text, float width, float height, std::vector<Colour> colours, std::vector<Texture*> textures) : GUICheckBox(text, width, height, colours, textures) {}

	/* The destructor */
	virtual ~GUIRadioCheckBox() {}

	/* The method called when the component state has changed */
	virtual void onChangeState() override;
};



#endif /* CORE_GUI_GUIRADIOCHECKBOX_H_ */
