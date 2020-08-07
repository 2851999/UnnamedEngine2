#version 420
#define UE_BLOOM


#define UE_DEFERRED_LIGHTING
//Used for assigning UBO block locations - can remove

/* NOTES:
* Set 0 - Varies per camera
* Set 1 - Varies per material
* Set 2 - Varies per model
* Set 3 - Varies per light batch
*/

layout(std140, set = 0, binding = 21) uniform UECameraData {
	mat4 ue_viewMatrix;
	mat4 ue_projectionMatrix;
	
	vec4 ue_cameraPosition;
};

layout(std140, set = 2, binding = 22) uniform UEModelData {
	mat4 ue_mvpMatrix;
	mat4 ue_modelMatrix;
	mat4 ue_normalMatrix;
};

layout(location = 0) in vec3 ue_frag_position;
layout(location = 1) in vec2 ue_frag_textureCoord;
layout(location = 2) in vec3 ue_frag_normal;
/* The material structure */
struct UEMaterial {
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;
	
	bool hasAmbientTexture;
	bool hasDiffuseTexture;
	bool diffuseTextureSRGB;
	bool hasSpecularTexture;
	bool hasShininessTexture;
	bool hasNormalMap;
	bool hasParallaxMap;
	
	float parallaxScale;
	float shininess;
};

/* The material data */
layout(std140, set = 1, binding = 23) uniform UEMaterialData {
	UEMaterial ue_material;
};

/* The texture data */
layout(set = 1, binding = 0) uniform sampler2D ue_material_ambientTexture;
layout(set = 1, binding = 1) uniform sampler2D ue_material_diffuseTexture;
layout(set = 1, binding = 2) uniform sampler2D ue_material_specularTexture;
layout(set = 1, binding = 3) uniform sampler2D ue_material_shininessTexture;
layout(set = 1, binding = 4) uniform sampler2D ue_material_normalMap;
layout(set = 1, binding = 5) uniform sampler2D ue_material_parallaxMap;

/* Various methods to get colours */
vec3 ueGetMaterialAmbient(vec2 textureCoord) {
	vec3 ambientColour = vec3(ue_material.ambientColour);
	if (ue_material.hasAmbientTexture)
		ambientColour *= texture(ue_material_ambientTexture, textureCoord).rgb;
	return ambientColour;
}

vec4 ueGetMaterialDiffuse(vec2 textureCoord) {
	vec4 diffuseColour = ue_material.diffuseColour;
	if (ue_material.hasDiffuseTexture) {
		// vec4 tex = texture(ue_material.diffuseTexture, textureCoord);
		// if (ue_material.diffuseTextureSRGB)
		// 	tex = pow(tex, vec4(2.2));
		// diffuseColour *= tex;
		diffuseColour *= texture(ue_material_diffuseTexture, textureCoord);
	}
	return diffuseColour;
}

vec3 ueGetMaterialSpecular(vec2 textureCoord) {
	vec3 specularColour = vec3(ue_material.specularColour);
	if (ue_material.hasSpecularTexture)
		specularColour *= texture(ue_material_specularTexture, textureCoord).rgb;
	return specularColour;
}

float ueGetMaterialShininess(vec2 textureCoord) {
	if (ue_material.hasShininessTexture)
		return texture(ue_material_shininessTexture, textureCoord).r;
	else
		return ue_material.shininess;
}

vec3 ueGetMaterialNormal(vec2 textureCoord) {
	vec3 normal = texture(ue_material_normalMap, textureCoord).rgb;
	normal.y = 1 - normal.y;
	normal = normalize(normal * 2.0 - 1.0);
	return normal;
}

vec2 ueGetMaterialParallax(vec2 textureCoord, vec3 viewDir) {
	const float minLayers = 10;
	const float maxLayers = 20;

	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	
	float layerSize = 1.0 / numLayers;
	float currentLayerDepth = 0.0;
	//Amount to move the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy / viewDir.z * ue_material.parallaxScale; 
	vec2 deltaTexCoords = P / numLayers;
  
	//Initial values
	vec2  currentTexCoords = textureCoord;
	float currentDepthMapValue = texture(ue_material_parallaxMap, currentTexCoords).r;
	
	while(currentLayerDepth < currentDepthMapValue) {
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(ue_material_parallaxMap, currentTexCoords).r;  
		currentLayerDepth += layerSize;
	}
	
	//Parallax occlusion mapping interpolation
	
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	
	float nextDepth = currentDepthMapValue - currentLayerDepth;
	float lastDepth = texture(ue_material_parallaxMap, prevTexCoords).r - currentLayerDepth + layerSize;
	float weight = nextDepth / (nextDepth - lastDepth);
	
	return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
}
#define MAX_LIGHTS 6

struct UELight {
	int  type;
	
	vec4 position;
	vec4 direction;
	vec4 diffuseColour;
	vec4 specularColour;
	
	float constant;
	float linear;
	float quadratic;
	
	float innerCutoff;
	float outerCutoff;
	
	bool useShadowMap;
};

layout(std140, set = 3, binding = 25) uniform UELightBatchData {
	UELight ue_lights[MAX_LIGHTS];
	mat4 ue_lightSpaceMatrix[MAX_LIGHTS];
	
	vec4 ue_lightAmbient;
	int ue_numLights;
	
	bool ue_useEnvironmentMap;
};

#ifndef UE_DEFERRED_LIGHTING
layout(location = 7) in vec3 ue_tangentViewPos;
layout(location = 8) in vec3 ue_tangentFragPos;

layout(location = 9) in mat3 ue_frag_tbnMatrix;
layout(location = 13) in vec4 ue_frag_pos_lightspace[MAX_LIGHTS];
#endif

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

#ifndef UE_DEFERRED_LIGHTING
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
#endif

const float PI = 3.14159265359;

//Wont work on all drivers
float radicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint N) {
	return vec2(float(i) / float(N), radicalInverse_VdC(i));
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    //Spherical coordinates to cartesian
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    //Tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float geometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

//Returns ratio of light that gets reflected on a surface (k_S)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

//Need to ensure doesn't conflict with deferred geometry buffer textures
//In the case of forward rendering the lighting shadow maps will have the
//greatest impact
#ifndef UE_NO_DEFERRED
layout(set = 5, binding = 20) uniform samplerCube ue_irradianceMap;
layout(set = 5, binding = 21) uniform samplerCube ue_prefilterMap;
layout(set = 5, binding = 22) uniform sampler2D   ue_brdfLUT;
#else
layout(set = 4, binding = 20) uniform samplerCube ue_irradianceMap;
layout(set = 4, binding = 21) uniform samplerCube ue_prefilterMap;
layout(set = 4, binding = 22) uniform sampler2D   ue_brdfLUT;
#endif

vec3 ueCalculateLightPBR(UELight light, vec3 lightDirection, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightColor = light.diffuseColour.xyz;

    //Calculate radiance
    vec3 L = lightDirection;
    vec3 H = normalize(viewDirection + L);
    vec3 radiance = lightColor;

    //Cook-torrance brdf
    float NDF = distributionGGX(normal, H, roughness);
    float G = geometrySmith(normal, viewDirection, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, viewDirection), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS; //Ratio of refraction

    kD *= 1.0 - metalness;

    float NdotL = max(dot(normal, L), 0.0);

    return  (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 ueCalculateDirectionalLightPBR(UELight light, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightDirection = normalize(-light.direction.xyz);

    return ueCalculateLightPBR(light, lightDirection, normal, viewDirection, fragPos, albedo, metalness, roughness, F0);
}

vec3 ueCalculatePointLightPBR(UELight light, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightPosition = light.position.xyz;

    vec3 lightDirection = normalize(lightPosition - fragPos);

    vec3 radiance = ueCalculateLightPBR(light, lightDirection, normal, viewDirection, fragPos, albedo, metalness, roughness, F0);

    float distanceToLight = length(lightPosition - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight); //Inverse square - need gamma correction

    radiance *= attenuation;

    return radiance;
}

vec3 ueCalculateSpotLightPBR(UELight light, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightPosition = light.position.xyz;

    //Calculate radiance
    vec3 lightDirection = normalize(lightPosition - fragPos);

    float theta = dot(lightDirection, normalize(-light.direction.xyz));

    if (theta > light.outerCutoff) {
        float e = light.innerCutoff - light.outerCutoff;
        float intensity = clamp((theta - light.outerCutoff) / e, 0.0, 1.0);

        //Attenuation
        float distanceToLight = length(lightPosition - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight); //Inverse square - need gamma correction

        vec3 radiance = ueCalculateLightPBR(light, lightDirection, normal, viewDirection, fragPos, albedo, metalness, roughness, F0);
        radiance *= intensity * attenuation;

        return radiance;
    } else {
        return vec3(0.0);
    }
}

//Returns the result of applying all lighting calculations
vec3 ueGetLightingPBR(vec3 normal, vec3 fragPos, vec3 albedo, float metalness, float roughness, float ao, vec4 fragPosLightSpace[MAX_LIGHTS]) {
    //View direction
    vec3 V = normalize(ue_cameraPosition.xyz - fragPos);

    vec3 R = reflect(-V, normal); 

    //Surface reflection at 0 incidence (how much surface reflects looking directly at the surface)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalness);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < ue_numLights; ++i) {
        if (ue_lights[i].type == 1) {
            if (ue_lights[i].useShadowMap)
                Lo += ueCalculateDirectionalLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0) * (1.0 - ueCalculateShadow(ue_lights[i], ue_lightTexturesShadowMap[i], fragPosLightSpace[i], normal));
            else
                Lo += ueCalculateDirectionalLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
        } else if (ue_lights[i].type == 2) {
			if (ue_lights[i].useShadowMap)
				Lo += ueCalculatePointLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0) * (1.0 - ueCalculatePointShadow(ue_lights[i], ue_lightTexturesShadowCubemap[i], fragPos, ue_cameraPosition.xyz));
			else
				Lo += ueCalculatePointLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
        } else if (ue_lights[i].type == 3)
			if (ue_lights[i].useShadowMap)
				Lo += ueCalculateSpotLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0) * (1.0 - ueCalculateShadow(ue_lights[i], ue_lightTexturesShadowMap[i], fragPosLightSpace[i], normal));
			else
				Lo += ueCalculateSpotLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
    }

    //vec3 ambient = vec3(0.03) * albedo * ao;

    //Ambient lighting
    vec3 ambient = vec3(0.0);

    if (ue_lightAmbient.r > 0.0) {
	    vec3 F = fresnelSchlickRoughness(max(dot(normal, V), 0.0), F0, roughness);
	    vec3 kS = F;
	    vec3 kD = 1.0 - kS;
	    kD *= 1.0 - metalness;
	    vec3 irradiance = texture(ue_irradianceMap, normal).rgb;
	    vec3 diffuse = irradiance * albedo;

	    const float MAX_REFLECTION_LOD = 4.0;
	    vec3 prefilteredColor = textureLod(ue_prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;    
	    vec2 brdf  = texture(ue_brdfLUT, vec2(max(dot(normal, V), 0.0), roughness)).rg;
	    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

	    ambient = (kD * diffuse + specular) * ao;
	}

    return ambient + Lo;
}

layout(set = 4, binding = 0) uniform sampler2D ue_gPosition;
layout(set = 4, binding = 1) uniform sampler2D ue_gNormal;
layout(set = 4, binding = 2) uniform sampler2D ue_gAlbedo;
layout(set = 4, binding = 3) uniform sampler2D ue_gMetalnessAO;

layout(location = 0) out vec4 ue_FragColour;

#ifdef UE_BLOOM
layout(location = 1) out vec4 ue_FragColourBright;
#endif

void main() {
	vec3 fragPosition = texture(ue_gPosition, ue_frag_textureCoord).rgb;
	vec4 albedo = texture(ue_gAlbedo, ue_frag_textureCoord);
	
	vec4 normalRough = texture(ue_gNormal, ue_frag_textureCoord);
	vec3 normal = normalRough.rgb;
	float roughness = normalRough.a;

	vec3 metalnessAO = texture(ue_gMetalnessAO, ue_frag_textureCoord).rgb;
	float metalness = metalnessAO.r;
	float ao = metalnessAO.g;

	vec4 fragPosLightSpace[MAX_LIGHTS];

	for (int i = 0; i < ue_numLights; i++) {
		if (ue_lights[i].useShadowMap && ue_lights[i].type != 2)
			fragPosLightSpace[i] = ue_lightSpaceMatrix[i] * vec4(fragPosition, 1.0);
	}
	
    vec3 colour = ueGetLightingPBR(normal, fragPosition, albedo.rgb, metalness, roughness, ao, fragPosLightSpace);
	
	ue_FragColour = vec4(colour, albedo.a);
	
	/*float maxDistance = stepValue * maxSteps;
	float dist = length(viewPos);
	if (dist < maxDistance)
		ue_FragColour = vec4(colour, 1.0);
	else
		ue_FragColour = vec4(colour * 0.1, 1.0);*/
	
	//ue_FragColour = vec4(vec3(ue_lights[0].quadratic), 1.0);
	//ue_FragColour = vec4(vec3(ueCalculatePointShadow(ue_lights[0], fragPosition, ue_cameraPosition)), 1.0);

	//Bloom
#ifdef UE_BLOOM
	float brightness = dot(albedo.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		ue_FragColourBright = vec4(albedo.rgb, 1.0);
	} else
		ue_FragColourBright = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}
