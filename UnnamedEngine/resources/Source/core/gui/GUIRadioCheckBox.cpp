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

#include "GUIRadioCheckBox.h"

#include "GUIGroup.h"

/*****************************************************************************
 * The GUIRadioCheckBox class
 *****************************************************************************/

void GUIRadioCheckBox::onChangeState() {
	if (mouseClicked) {
		//Check whether this check box is not current checked
		if (! checked) {
			//Check whether this is part of a group
			if (hasGroup()) {
				//Go through all of the other radio check boxes part of the group
				for (GUIComponent* current : getGroup()->getComponents()) {
					//Attempt to cast it to another radio check box
					if (GUICheckBox* checkBox = dynamic_cast<GUICheckBox*>(current)) {
						//Make sure that check box
						checkBox->setChecked(false);
					}
				}
			}
			//Make this check box checked
			checked = true;
		}
	}
	updateRenderIndex();
}
