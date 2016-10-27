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
#map uniform Camera_Right cameraRight
#map uniform Camera_Up cameraUp
#map attribute Position position
#map attribute PositionsData positionsData
#map attribute Colour colour
#map attribute TextureData textureData

//Matrices
uniform mat4 mvpMatrix;

//Bilboarding data
uniform vec3 cameraRight;
uniform vec3 cameraUp;

//Mesh data
in vec3 position;		//Current position of the particle mesh
in vec4 positionsData;  //Contains the position, and size of the current particle
in vec4 colour;			//The colour of the current particle
in vec4 textureData;	//Contains the top, left, bottom and right coordinates of the texture for the current particle

out vec4 frag_colour;
out vec2 frag_textureCoord;

void main() {
	//Get the needed data
	vec3 centre = positionsData.xyz;
	float billboardSize = positionsData.w;
	
	float top = textureData.x;
	float left = textureData.y;
	float bottom = textureData.z;
	float right = textureData.w;
	
	//Assign the values needed for the fragment shader
	frag_colour = colour;
	frag_textureCoord = vec2(((position.x + 0.5) * right) - ((position.x - 0.5) * left), ((position.y + 0.5) * top) - ((position.y - 0.5) * bottom));
	
	//Assign the position of the vertex
	gl_Position = mvpMatrix * vec4(centre + ((cameraRight * position.x * billboardSize) + (cameraUp * position.y * billboardSize)), 1.0);
}