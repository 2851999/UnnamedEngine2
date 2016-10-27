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

/* The material structure */
struct Material {
	vec3 ambientColour;
	vec4 diffuseColour;
	vec3 specularColour;
	
	sampler2D ambientTexture;
	sampler2D diffuseTexture;
	sampler2D specularTexture;
	
	sampler2D normalMap;
	
	float shininess;
};

//The material to use
uniform Material material;

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

//The number of lights actually assigned
uniform int numLights;
//The ambient light to use
uniform vec3 light_ambient;
//The current camera position
uniform vec3 camera_position;

//The environment map (experimental)
uniform samplerCube environmentMap;
uniform bool useEnvironmentMap;

//States whether the normal map should be used
uniform bool useNormalMap;

//The shadow map
uniform sampler2D shadowMap;
uniform bool useShadowMap;

//Data from the vertex shader
in vec3 frag_position;
in vec2 frag_textureCoord;
in vec3 frag_normal;
in vec4 frag_pos_lightspace;

in mat3 frag_tbnMatrix;

//The final colour
out vec4 FragColour;

//Returns the result of directional lighting calculations
vec3 calculateDirectionalLight(Light light, vec3 diffuseColour, vec3 specularColour, vec3 normal) {
	vec3 lightDirection = normalize(-light.direction);
	
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light.diffuseColour * diffuseColour);
	
	vec3 viewDirection = normalize(camera_position - frag_position);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	
	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);
	vec3 specularLight = specularStrength * (light.specularColour * specularColour);
	
	return diffuseLight + specularLight;
}

//Returns the result of point lighting calculations
vec3 calculatePointLight(Light light, vec3 diffuseColour, vec3 specularColour, vec3 normal) {
	vec4 diffuseTexColour = texture(material.diffuseTexture, frag_textureCoord);
	
	vec3 lightDirection = normalize(light.position - frag_position);
	
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light.diffuseColour * diffuseColour);
	
	vec3 viewDirection = normalize(camera_position - frag_position);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		
	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);
	vec3 specularLight = specularStrength * (light.specularColour * specularColour);

	float distanceToLight = length(light.position - frag_position);
	float attentuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
	
	diffuseLight *= attentuation;
	specularLight *= attentuation;
	
	return diffuseLight + specularLight;
}

//Returns the result of spot lighting calculations
vec3 calculateSpotLight(Light light, vec3 diffuseColour, vec3 specularColour, vec3 normal) {
	vec3 lightDirection = normalize(light.position - frag_position);
	
	float theta = dot(lightDirection, normalize(-light.direction));
	
	if (theta > light.outerCutoff) {
		float e = light.cutoff - light.outerCutoff;
		float intensity = clamp((theta - light.outerCutoff) / e, 0.0, 1.0);

		float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
		vec3 diffuseLight = diffuseStrength * (light.diffuseColour * diffuseColour);
		
		vec3 viewDirection = normalize(camera_position - frag_position);
		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		
		float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), material.shininess);
		vec3 specularLight = specularStrength * (light.specularColour * specularColour);
		
		diffuseLight *= intensity;
		specularLight *= intensity;
		
		return diffuseLight + specularLight;
	} else {
		return vec3(0.0);
	}
}

//Returns a float value which states how much in shadow a particular fragment is
float calculateShadow(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal) {
	//Perspective divide
	vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projectedCoords = projectedCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projectedCoords.xy).r;
	float currentDepth = projectedCoords.z;
	
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
//	float shadow = 0.0;
//	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//	for(int x = -1; x <= 1; ++x) {
//		for(int y = -1; y <= 1; ++y) {
//			float pcfDepth = texture(shadowMap, projectedCoords.xy + vec2(x, y) * texelSize).r; 
//			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
//		}    
//	}
//	shadow /= 9.0;
	
	return shadow;
}

void main() {
	//Calculate the colours
	vec3 ambientColour = material.ambientColour * texture(material.diffuseTexture, frag_textureCoord).xyz;
	vec4 diffuseColour = material.diffuseColour * texture(material.diffuseTexture, frag_textureCoord);
	vec3 specularColour = material.specularColour * texture(material.specularTexture, frag_textureCoord).xyz;
	
	//The ambient light
	vec3 ambientLight = light_ambient * ambientColour;
	
	//The diffuse and specular light from the lighting calculations
	vec3 otherLight = vec3(0.0);
	
	if (useEnvironmentMap) {
		vec3 I = normalize(frag_position - camera_position);
		vec3 R = reflect(I, normalize(frag_normal));
		
		ambientColour *= texture(environmentMap, R).rgb;
		diffuseColour *= texture(environmentMap, R);
		specularColour *= texture(environmentMap, R).rgb;
	}
	
	//Calculate the normal
	vec3 normal;
	
	if (useNormalMap) {
		//Get the normal from the texture
		normal = texture(material.normalMap, frag_textureCoord).rgb;
		normal.y = 1 - normal.y;
		
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(frag_tbnMatrix * normal);
	} else {
		normal = normalize(frag_normal);
	}
	
	//Go through the lights
	for (int i = 0; i < numLights; i++) {
		//Check the light type
		if (lights[i].type == 1) {
			if (useShadowMap)
				otherLight += calculateDirectionalLight(lights[i], diffuseColour.xyz, specularColour, normal) * (1.0 - calculateShadow(frag_pos_lightspace, lights[i].direction, normal));
			else
				otherLight += calculateDirectionalLight(lights[i], diffuseColour.xyz, specularColour, normal);
		} else if (lights[i].type == 2)
			otherLight += calculatePointLight(lights[i], diffuseColour.xyz, specularColour, normal);
		else if (lights[i].type == 3)
			otherLight += calculateSpotLight(lights[i], diffuseColour.xyz, specularColour, normal);
	}
	
	//Assign the colour
	FragColour = vec4(ambientLight + otherLight, 1.0);
}