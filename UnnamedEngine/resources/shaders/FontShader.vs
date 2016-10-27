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

#map uniform MVPMatrix mvpMatrix
#map attribute Position position
#map attribute TextureCoordinate textureCoord

//Matrices
uniform mat4 mvpMatrix;

//Mesh data
in vec3 position;
in vec2 textureCoord;

//Data for the fragment shader
out vec2 frag_textureCoord;

void main() {
	//Assign the needed data for the fragment shader
	frag_textureCoord = textureCoord;
	
	//Assign the vertex position
	gl_Position = mvpMatrix * vec4(position, 1.0);
}