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

#ifndef CORE_RENDER_CUBEMAP_H_
#define CORE_RENDER_CUBEMAP_H_

#include "Texture.h"

/*****************************************************************************
 * The Cubemap class inherits from Texture to create a cubemap
 *****************************************************************************/

class Cubemap : public Texture {
public:
	/* The constructor */
	Cubemap(std::string path, std::vector<std::string> faces);
};


#endif /* CORE_RENDER_CUBEMAP_H_ */
