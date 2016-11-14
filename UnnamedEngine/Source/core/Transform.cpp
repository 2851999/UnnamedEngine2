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

#include "Transform.h"

/*****************************************************************************
 * The Transform class
 *****************************************************************************/

void Transform::calculateMatrix(Vector3f offset) {
	matrix.setIdentity();
	matrix.translate(position + offset);
	matrix.rotate(rotation);
	matrix.translate(offset * -1);
	matrix.scale(scale);
}

void Transform::rotate(const Quaternion& rotation) {
	setRotation(Quaternion((rotation * this->localRotation).normalise()));
}
