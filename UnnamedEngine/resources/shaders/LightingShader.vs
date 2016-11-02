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

#define MAX_LIGHTS 6

//The light structure contains all data that may be needed for a light
struct Light {
	int  type;

	vec3 position;
	vec3 direction;
	
	vec3 diffuseColour;
	vec3 specularColour;
	
	float constant;
	float linear;
	float quadratic;
	
	float cutoff;
	float outerCutoff;
};

//The lights to render
uniform Light lights[MAX_LIGHTS];

//Matrices
uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

uniform mat4 lightSpaceMatrix; //Used for shadow mapping

//States whether normal mapping should be used
uniform bool useNormalMap;

//Mesh data
in vec3 position;
in vec2 textureCoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

//Data for the fragment shader
out vec3 frag_position;
out vec2 frag_textureCoord;
out vec3 frag_normal;
out vec4 frag_pos_lightspace;

out mat3 frag_tbnMatrix;

void main() {
	//Assign the needed data for the fragment shader
	frag_position = vec3(modelMatrix * vec4(position, 1.0));
	frag_textureCoord = textureCoord;
	frag_normal = normalMatrix * normal;
	
	frag_pos_lightspace = lightSpaceMatrix * vec4(frag_position, 1.0);
	
	if (useNormalMap) {
		vec3 T = normalize(normalMatrix * tangent);
		vec3 B = normalize(normalMatrix * bitangent);
		vec3 N = normalize(frag_normal);
	
		frag_tbnMatrix = mat3(T, B, N);
	}
	
	//Assign the vertex position
	gl_Position = mvpMatrix * vec4(position, 1.0);
}