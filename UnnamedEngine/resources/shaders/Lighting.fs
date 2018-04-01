#include "Core.fs"
#include "Material.fs"

#define MAX_LIGHTS 6

struct UELight {
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

	sampler2D shadowMap;
	bool useShadowMap;
};

uniform UELight ue_lights[MAX_LIGHTS];

uniform int ue_numLights;

uniform vec3 ue_lightAmbient;

uniform samplerCube ue_environmentMap;
uniform bool ue_useEnvironmentMap;
uniform bool ue_useNormalMap;
uniform bool ue_useParallaxMap;

in vec4 ue_frag_pos_lightspace[MAX_LIGHTS];
in mat3 ue_frag_tbnMatrix;

in vec3 ue_tangentViewPos;
in vec3 ue_tangentFragPos;

//Returns the result of directional lighting calculations
vec3 ueCalculateDirectionalLight(UELight light, vec3 diffuseColour, vec3 specularColour, vec3 normal, vec3 fragPos, float matShininess) {
	vec3 lightDirection = normalize(-light.direction);
	
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light.diffuseColour * diffuseColour);
	
	vec3 viewDirection = normalize(ue_cameraPosition - fragPos);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	
	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), matShininess);
	vec3 specularLight = specularStrength * (light.specularColour * specularColour);
	
	return diffuseLight + specularLight;
}

//Returns the result of point lighting calculations
vec3 ueCalculatePointLight(UELight light, vec3 diffuseColour, vec3 specularColour, vec3 normal, vec3 fragPos, float matShininess) {
	vec3 lightDirection = normalize(light.position - fragPos);
	
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light.diffuseColour * diffuseColour);
	
	vec3 viewDirection = normalize(ue_cameraPosition - fragPos);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		
	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), matShininess);
	vec3 specularLight = specularStrength * (light.specularColour * specularColour);

	float distanceToLight = length(light.position - fragPos);
	float attentuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
	
	diffuseLight *= attentuation;
	specularLight *= attentuation;
	
	return diffuseLight + specularLight;
}

//Returns the result of spot lighting calculations
vec3 ueCalculateSpotLight(UELight light, vec3 diffuseColour, vec3 specularColour, vec3 normal, vec3 fragPos, float matShininess) {
	vec3 lightDirection = normalize(light.position - fragPos);
	
	float theta = dot(lightDirection, normalize(-light.direction));
	
	if (theta > light.outerCutoff) {
		float e = light.cutoff - light.outerCutoff;
		float intensity = clamp((theta - light.outerCutoff) / e, 0.0, 1.0);

		float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
		vec3 diffuseLight = diffuseStrength * (light.diffuseColour * diffuseColour);
		
		vec3 viewDirection = normalize(ue_cameraPosition - fragPos);
		vec3 halfwayDirection = normalize(lightDirection + viewDirection);
		
		float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), matShininess);
		vec3 specularLight = specularStrength * (light.specularColour * specularColour);
		
		diffuseLight *= intensity;
		specularLight *= intensity;
		
		return diffuseLight + specularLight;
	} else {
		return vec3(0.0);
	}
}

//Returns a float value which states how much in shadow a particular fragment is
float ueCalculateShadow(UELight light, vec4 fragPosLightSpace, vec3 normal) {
	//Perspective divide
	vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projectedCoords = projectedCoords * 0.5 + 0.5;
	//float closestDepth = texture(light.shadowMap, projectedCoords.xy).r;
	float currentDepth = projectedCoords.z;
	
	float bias = max(0.01 * (1.0 - dot(normal, light.direction)), 0.005);
	
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(light.shadowMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(light.shadowMap, projectedCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	
	return shadow;
}

//Returns the result of applying all lighting calculations
vec3 ueGetLighting(vec3 normal, vec3 fragPos, vec3 ambientColour, vec3 diffuseColour, vec3 specularColour, float matShininess, vec4 fragPosLightSpace[MAX_LIGHTS]) {
	//The ambient light
	vec3 ambientLight = ue_lightAmbient * ambientColour;
	
	//The diffuse and specular light from the lighting calculations
	vec3 otherLight = vec3(0.0);
	
	if (ue_useEnvironmentMap) {
		vec3 I = normalize(fragPos - ue_cameraPosition);
		vec3 R = reflect(I, normalize(normal));
		
		ambientColour *= texture(ue_environmentMap, R).rgb;
		diffuseColour *= texture(ue_environmentMap, R).rgb;
		specularColour *= texture(ue_environmentMap, R).rgb;
	}
	
	//Go through the lights
	for (int i = 0; i < ue_numLights; i++) {
		//Check the light type
		if (ue_lights[i].type == 1) {
			if (ue_lights[i].useShadowMap)
				otherLight += ueCalculateDirectionalLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess) * (1.0 - ueCalculateShadow(ue_lights[i], fragPosLightSpace[i], normal));
			else
				otherLight += ueCalculateDirectionalLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess);
		} else if (ue_lights[i].type == 2)
			otherLight += ueCalculatePointLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess);
		else if (ue_lights[i].type == 3)
			otherLight += ueCalculateSpotLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess);
	}

	return ambientLight + otherLight;
}

//Returns the texture coordinate taking into account parralax mapping
vec2 ueCalculateTextureCoord() {
	vec2 textureCoord = ue_frag_textureCoord;
	if (ue_useParallaxMap) {
		textureCoord = ueGetMaterialParallax(ue_frag_textureCoord, normalize(ue_tangentViewPos - ue_tangentFragPos));
		
		if(textureCoord.x > 1.0 || textureCoord.y > 1.0 || textureCoord.x < 0.0 || textureCoord.y < 0.0)
			discard;
	}
	return textureCoord;
}

//Returns the normal taking into account normal mapping
vec3 ueCalculateNormal(vec2 textureCoord) {
	vec3 normal;
	
	if (ue_useNormalMap)
		normal = normalize(ue_frag_tbnMatrix * ueGetMaterialNormal(textureCoord));
	else
		normal = normalize(ue_frag_normal);

	return normal;
}

vec3 ueGammaCorrect(vec3 colour) {
	const float gamma = 2.2;
	return pow(colour, vec3(1.0 / gamma));
}

vec3 ueReinhardToneMapping(vec3 colour) {
	vec3 mapped = colour / (colour + vec3(1.0));

	return mapped;
}

vec3 ueExposureToneMap(float exposure, vec3 colour) {
	vec3 mapped = vec3(1.0) - exp(-colour * exposure);

	return mapped;
}