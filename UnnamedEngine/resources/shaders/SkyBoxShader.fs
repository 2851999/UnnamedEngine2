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

#version 140

#map uniform Material_DiffuseTexture tex

//The SkyBox cubemap texture
uniform samplerCube tex;

//Data from the vertex shader
in vec3 frag_textureCoord;

//The final colour
out vec4 FragColor;

void main() {
	//Assign the colour
	FragColor = texture(tex, frag_textureCoord);
}