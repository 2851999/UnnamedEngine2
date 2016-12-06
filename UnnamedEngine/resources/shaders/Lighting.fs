#include "Core.fs"
#include "Material.fs"

#define MAX_LIGHTS 6

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

uniform Light lights[MAX_LIGHTS];

uniform int numLights;

uniform vec3 light_ambient;

uniform samplerCube environmentMap;
uniform bool useEnvironmentMap;
uniform bool useNormalMap;

uniform sampler2D shadowMap;
uniform bool useShadowMap;

in vec3 frag_normal;
in vec4 frag_pos_lightspace;
in mat3 frag_tbnMatrix;

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
	
	float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);
	
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(shadowMap, projectedCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	
	return shadow;
}

vec3 ue_getLighting(vec3 normal, vec3 ambientColour, vec3 diffuseColour, vec3 specularColour) {
	//The ambient light
	vec3 ambientLight = light_ambient * ambientColour;
	
	//The diffuse and specular light from the lighting calculations
	vec3 otherLight = vec3(0.0);
	
	if (useEnvironmentMap) {
		vec3 I = normalize(frag_position - camera_position);
		vec3 R = reflect(I, normalize(frag_normal));
		
		ambientColour *= texture(environmentMap, R).rgb;
		diffuseColour *= texture(environmentMap, R).rgb;
		specularColour *= texture(environmentMap, R).rgb;
	}
	
	//Go through the lights
	for (int i = 0; i < numLights; i++) {
		//Check the light type
		if (lights[i].type == 1) {
			if (useShadowMap)
				otherLight += calculateDirectionalLight(lights[i], diffuseColour, specularColour, normal) * (1.0 - calculateShadow(frag_pos_lightspace, lights[i].direction, normal));
			else
				otherLight += calculateDirectionalLight(lights[i], diffuseColour, specularColour, normal);
		} else if (lights[i].type == 2)
			otherLight += calculatePointLight(lights[i], diffuseColour, specularColour, normal);
		else if (lights[i].type == 3)
			otherLight += calculateSpotLight(lights[i], diffuseColour, specularColour, normal);
	}
	
	return ambientLight + otherLight;
}