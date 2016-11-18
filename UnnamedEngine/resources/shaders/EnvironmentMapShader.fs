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

#map uniform Material_DiffuseColour material.diffuseColour
#map uniform Material_DiffuseTexture material.diffuseTexture
#map uniform EnvironmentMap environmentMap
#map uniform CameraPosition cameraPos

/* The material structure */
struct Material {
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;
	
	sampler2D ambientTexture;
	sampler2D diffuseTexture;
	sampler2D specularTexture;
};

//The material for the mesh
uniform Material material;
//The environment map to apply
uniform samplerCube environmentMap;
//The current camera position
uniform vec3 cameraPos;

//Data from the vertex shader
in vec2 frag_textureCoord;
in vec3 frag_normal;
in vec3 frag_position;

//The final colour
out vec4 FragColour;

void main() {
	//Calculate the incident vector
	vec3 I = normalize(frag_position - cameraPos);
	//Now calculate the reflection vector by reflecting about the mesh's normal
	vec3 R = reflect(I, normalize(frag_normal));
	
	//vec3 R = refract(I, normalize(frag_normal), 1.00 / 1.52);
	
	//FragColour = material.diffuseColour * texture2D(material.diffuseTexture, frag_textureCoord) * texture(environmentMap, R);
	
	//Assign the colour
	FragColour = vec4(vec3(material.diffuseColour * texture(material.diffuseTexture, frag_textureCoord) * texture(environmentMap, R)), 1.0);
}