#include "../Core.fs"
#include "../Material.fs"
#include "Lighting.glsl"

layout(location = 7) in vec3 ue_tangentViewPos;
layout(location = 8) in vec3 ue_tangentFragPos;

layout(location = 9) in mat3 ue_frag_tbnMatrix;
layout(location = 13) in vec4 ue_frag_pos_lightspace[MAX_LIGHTS];

//layout(binding = 6) uniform samplerCube ue_environmentMap;

layout(set = 3, binding = 7) uniform sampler2D ue_lightTexturesShadowMap[MAX_LIGHTS];
layout(set = 3, binding = 13) uniform samplerCube ue_lightTexturesShadowCubemap[MAX_LIGHTS];

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
float ueCalculateShadow(UELight light, sampler2D shadowMap, vec4 fragPosLightSpace, vec3 normal) {
	//Perspective divide
	vec3 projectedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//Now in range -1 to 1
	projectedCoords.xy = projectedCoords.xy * 0.5 + 0.5;
	//float closestDepth = texture(shadowMap, projectedCoords.xy).r;

	/*
	In Vulkan, depth from shadow map will be between 0 and 1, where as OpenGL will also give between 0 and 1
	Here looking up texture, find depth values between 0 and 1, however projectedCoords.z will have been converted
	so that in OpenGL they will be between -1 and 1 => Need conversion
	But Vulkan will be in range 0 to 1 => Nothing needs to be done

		float currentDepth = projectedCoords.z; //VULKAN
		float currentDepth = projectedCoords.z * 0.5 + 0.5; //OPENGL
	*/

	#ifdef VULKAN
		float currentDepth = projectedCoords.z; //VULKAN
	#else
		float currentDepth = projectedCoords.z * 0.5 + 0.5; //OPENGL
	#endif
	
	float bias = max(0.01 * (1.0 - dot(normal, light.direction.xyz)), 0.005);
	
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

//Same as above, but for a point light
float ueCalculatePointShadow(UELight light, samplerCube shadowCubemap, vec3 fragPos, vec3 viewPos) {
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
		float closestDepth = texture(shadowCubemap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= farPlane;
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	
	return shadow /= float(samples);
	//return texture(shadowCubemap, fragToLight).r;
}

//Returns the result of applying all lighting calculations
vec3 ueGetLighting(vec3 normal, vec3 fragPos, vec3 ambientColour, vec3 diffuseColour, vec3 specularColour, float matShininess, vec4 fragPosLightSpace[MAX_LIGHTS]) {
	//The ambient light
	vec3 ambientLight = ue_lightAmbient.xyz * ambientColour;
	
	//The diffuse and specular light from the lighting calculations
	vec3 otherLight = vec3(0.0);
	
	/*if (ue_useEnvironmentMap) {
		vec3 I = normalize(fragPos - ue_cameraPosition.xyz);
		vec3 R = reflect(I, normalize(normal));
		
		ambientColour *= texture(ue_environmentMap, R).rgb;
		diffuseColour *= texture(ue_environmentMap, R).rgb;
		specularColour *= texture(ue_environmentMap, R).rgb;
	}*/
	
	//Go through the lights
	for (int i = 0; i < ue_numLights; i++) {
		UELight light = ue_lights[i];
		//Check the light type
		if (light.type == 1) {
			if (light.useShadowMap)
				otherLight += ueCalculateDirectionalLight(light, diffuseColour, specularColour, normal, fragPos, matShininess) * (1.0 - ueCalculateShadow(light, ue_lightTexturesShadowMap[i], fragPosLightSpace[i], normal));
			else
				otherLight += ueCalculateDirectionalLight(light, diffuseColour, specularColour, normal, fragPos, matShininess);
		} else if (light.type == 2) {
			if (light.useShadowMap)
				otherLight += ueCalculatePointLight(light, diffuseColour, specularColour, normal, fragPos, matShininess) * (1.0 - ueCalculatePointShadow(light, ue_lightTexturesShadowCubemap[i], fragPos, ue_cameraPosition.xyz));
			else
				otherLight += ueCalculatePointLight(light, diffuseColour, specularColour, normal, fragPos, matShininess);
		} else if (light.type == 3) {
			if (light.useShadowMap)
				otherLight += ueCalculateSpotLight(light, diffuseColour, specularColour, normal, fragPos, matShininess) * (1.0 - ueCalculateShadow(light, ue_lightTexturesShadowMap[i], fragPosLightSpace[i], normal));
			else
				otherLight += ueCalculateSpotLight(light, diffuseColour, specularColour, normal, fragPos, matShininess);
		}
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