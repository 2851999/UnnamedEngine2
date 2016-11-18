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
#map uniform ModelMatrix modelMatrix
#map uniform NormalMatrix normalMatrix
#map attribute Position position
#map attribute TextureCoordinate textureCoord
#map attribute Normal normalMatrix

//Matrices
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;

//Mesh data
in vec3 position;
in vec2 textureCoord;
in vec3 normal;

//Data for the fragment shader
out vec2 frag_textureCoord;
out vec3 frag_normal;
out vec3 frag_position;

void main() {
	//Pass the needed data to the fragment shader
	frag_textureCoord = textureCoord;
	frag_normal = normalMatrix * normal;
	frag_position = vec3(modelMatrix * vec4(position, 1.0f));
	
	//Assign the vertex position
	gl_Position = mvpMatrix * vec4(position, 1.0);
}