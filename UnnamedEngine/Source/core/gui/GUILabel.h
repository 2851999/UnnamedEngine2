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


#ifndef CORE_GUI_GUILABEL_H_
#define CORE_GUI_GUILABEL_H_

#include "GUIComponent.h"

/*****************************************************************************
 * The GUILabel class
 *****************************************************************************/

class GUILabel : public GUIComponent {
public:
	/* The constructors */
	GUILabel(std::string text, Font* font = NULL);

	/* The destructor */
	virtual ~GUILabel() {}

	/* The method called to render this component */
	virtual void render(bool overrideShader = false) override;
};

#endif /* CORE_GUI_GUILABEL_H_ */
