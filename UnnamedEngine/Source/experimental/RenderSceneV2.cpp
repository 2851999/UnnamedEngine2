/*****************************************************************************
 *
 *   Copyright 2020 Joel Davies
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

#include "RenderSceneV2.h"

RenderSceneV2::RenderSceneV2() {

}

RenderSceneV2::~RenderSceneV2() {
    //Go through and delete all created objects
    for (unsigned int i = 0; i < objects.size(); ++i)
        delete objects[i];
    objects.clear();
}

void RenderSceneV2::add(GameObject3D* object) {
    //Add the object to the scene
    objects.push_back(object);
}

void RenderSceneV2::render() {
    //Go through and render all of the objects
    for (unsigned int i = 0; i < objects.size(); ++i)
        objects[i]->render();
}