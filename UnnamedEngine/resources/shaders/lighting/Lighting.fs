#include "../Core.fs"
#include "../Material.fs"
#include "Lighting.glsl"

struct UELightTextures {
	sampler2D shadowMap;
	samplerCube shadowCubemap;
};

uniform UELightTextures ue_lightsTextures[MAX_LIGHTS];

uniform samplerCube ue_environmentMap;

in vec4 ue_frag_pos_lightspace[MAX_LIGHTS];
in mat3 ue_frag_tbnMatrix;

in vec3 ue_tangentViewPos;
in vec3 ue_tangentFragPos;

//Returns the sum of the specular and diffuse strengths */
vec3 ueCalculateLight(UELight light, vec3 lightDirection, vec3 diffuseColour, vec3 specularColour, vec3 normal, vec3 fragPos, float matShininess) {
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light.diffuseColour.xyz * diffuseColour);
	
	vec3 viewDirection = normalize(ue_cameraPosition.xyz - fragPos);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	
	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), matShininess);
	vec3 specularLight = specularStrength * (light.specularColour.xyz * specularColour);
	
	return diffuseLight + specularLight;
}

//Returns the result of directional lighting calculations
vec3 ueCalculateDirectionalLight(UELight light, vec3 diffuseColour, vec3 specularColour, vec3 normal, vec3 fragPos, float matShininess) {
	vec3 lightDirection = normalize(-light.direction.xyz);
	
	return ueCalculateLight(light, lightDirection, diffuseColour, specularColour, normal, fragPos, matShininess);
}

//Returns the result of point lighting calculations
vec3 ueCalculatePointLight(UELight light, vec3 diffuseColour, vec3 specularColour, vec3 normal, vec3 fragPos, float matShininess) {
	vec3 lightDirection = normalize(light.position.xyz - fragPos);
	
	vec3 diffuseSpecLight = ueCalculateLight(light, lightDirection, diffuseColour, specularColour, normal, fragPos, matShininess);

	float distanceToLight = length(light.position.xyz - fragPos);
	float attentuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
	
	diffuseSpecLight *= attentuation;
	
	return diffuseSpecLight;
}

//Returns the result of spot lighting calculations
vec3 ueCalculateSpotLight(UELight light, vec3 diffuseColour, vec3 specularColour, vec3 normal, vec3 fragPos, float matShininess) {
	vec3 lightDirection = normalize(light.position.xyz - fragPos);
	
	float theta = dot(lightDirection, normalize(-light.direction.xyz));
	
	if (theta > light.outerCutoff) {
		float e = light.innerCutoff - light.outerCutoff;
		float intensity = clamp((theta - light.outerCutoff) / e, 0.0, 1.0);

		vec3 diffuseSpecLight = ueCalculateLight(light, lightDirection, diffuseColour, specularColour, normal, fragPos, matShininess);

		//Calculate attenuation
		float distanceToLight = length(light.position.xyz - fragPos);
		float attentuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight);
		
		diffuseSpecLight *= intensity * attentuation;
		
		return diffuseSpecLight;
	} else {
		return vec3(0.0);
	}
}

//Returns a float value which states how much in shadow a particular fragment is
float ueCalculateShadow(UELight light, UELightTextures lightTextures, vec4 fragPosLightSpace, vec3 normal) {
	//Perspective divide
	vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projectedCoords = projectedCoords * 0.5 + 0.5;
	//float closestDepth = texture(light.shadowMap, projectedCoords.xy).r;
	float currentDepth = projectedCoords.z;
	
	float bias = max(0.01 * (1.0 - dot(normal, light.direction.xyz)), 0.005);
	
	//float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(lightTextures.shadowMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(lightTextures.shadowMap, projectedCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	
	return shadow;
}

//Same as above, but for a point light
float ueCalculatePointShadow(UELight light, UELightTextures lightTextures, vec3 fragPos, vec3 viewPos) {
	vec3 fragToLight = fragPos - light.position.xyz;
	float farPlane = 25.0;
	float currentDepth = length(fragToLight);
	
	vec3 sampleOffsetDirections[20] = vec3[](
		vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
		vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
		vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
		vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1) );
	
	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
	
	for (int i = 0; i < samples; ++i) {
		float closestDepth = texture(lightTextures.shadowCubemap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= farPlane;
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	
	return shadow /= float(samples);
	//return texture(light.shadowCubemap, fragToLight).r;
}

//Returns the result of applying all lighting calculations
vec3 ueGetLighting(vec3 normal, vec3 fragPos, vec3 ambientColour, vec3 diffuseColour, vec3 specularColour, float matShininess, vec4 fragPosLightSpace[MAX_LIGHTS]) {
	//The ambient light
	vec3 ambientLight = ue_lightAmbient.xyz * ambientColour;
	
	//The diffuse and specular light from the lighting calculations
	vec3 otherLight = vec3(0.0);
	
	if (ue_useEnvironmentMap) {
		vec3 I = normalize(fragPos - ue_cameraPosition.xyz);
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
				otherLight += ueCalculateDirectionalLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess) * (1.0 - ueCalculateShadow(ue_lights[i], ue_lightsTextures[i], fragPosLightSpace[i], normal));
			else
				otherLight += ueCalculateDirectionalLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess);
		} else if (ue_lights[i].type == 2) {
			if (ue_lights[i].useShadowMap)
				otherLight += ueCalculatePointLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess)* (1.0 - ueCalculatePointShadow(ue_lights[i], ue_lightsTextures[i], fragPos, ue_cameraPosition.xyz));
			else
				otherLight += ueCalculatePointLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess);
		} else if (ue_lights[i].type == 3)
			otherLight += ueCalculateSpotLight(ue_lights[i], diffuseColour, specularColour, normal, fragPos, matShininess);
	}

	return ambientLight + otherLight;
}

//Returns the texture coordinate taking into account parralax mapping
vec2 ueCalculateTextureCoord() {
	vec2 textureCoord = ue_frag_textureCoord;
	if (ue_material.hasParallaxMap) {
		textureCoord = ueGetMaterialParallax(ue_frag_textureCoord, normalize(ue_tangentViewPos - ue_tangentFragPos));
		
		if(textureCoord.x > 1.0 || textureCoord.y > 1.0 || textureCoord.x < 0.0 || textureCoord.y < 0.0)
			discard;
	}
	return textureCoord;
}

//Returns the normal taking into account normal mapping
vec3 ueCalculateNormal(vec2 textureCoord) {
	vec3 normal;
	
	if (ue_material.hasNormalMap)
		normal = normalize(ue_frag_tbnMatrix * ueGetMaterialNormal(textureCoord));
	else
		normal = normalize(ue_frag_normal);

	return normal;
}