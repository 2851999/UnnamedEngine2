#version 420
//Used for assigning UBO block locations - can remove

layout(std140, binding = 1) uniform UECoreData {
	mat4 ue_mvpMatrix;
	mat4 ue_modelMatrix;
	mat4 ue_viewMatrix;
	mat4 ue_projectionMatrix;
	mat4 ue_normalMatrix;
	
	vec4 ue_cameraPosition;
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
layout(std140, set = 1, binding = 2) uniform UEMaterialData {
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

layout(std140, binding = 4) uniform UELightingData {
	UELight ue_lights[MAX_LIGHTS];
	mat4 ue_lightSpaceMatrix[MAX_LIGHTS];
	
	vec4 ue_lightAmbient;
	int ue_numLights;
	
	bool ue_useEnvironmentMap;
};

layout(location = 7) in vec3 ue_tangentViewPos;
layout(location = 8) in vec3 ue_tangentFragPos;

layout(location = 9) in mat3 ue_frag_tbnMatrix;
layout(location = 13) in vec4 ue_frag_pos_lightspace[MAX_LIGHTS];

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
	projectedCoords = projectedCoords * 0.5 + 0.5;
	//float closestDepth = texture(shadowMap, projectedCoords.xy).r;
	float currentDepth = projectedCoords.z;
	
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
	
	//Go through the lights
	for (int i = 0; i < ue_numLights; i++) {
		UELight light = ue_lights[i];
		//Check the light type
		if (light.type == 1) {
			otherLight += ueCalculateDirectionalLight(light, diffuseColour, specularColour, normal, fragPos, matShininess);
		} else if (light.type == 2) {
			otherLight += ueCalculatePointLight(light, diffuseColour, specularColour, normal, fragPos, matShininess);
		} else if (light.type == 3)
			otherLight += ueCalculateSpotLight(light, diffuseColour, specularColour, normal, fragPos, matShininess);
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

layout(location = 0) out vec4 ue_FragColour;

void main() {
	vec2 textureCoord = ueCalculateTextureCoord();

	vec3 ambientColour = ueGetMaterialAmbient(textureCoord);
	vec4 diffuseColour = ueGetMaterialDiffuse(textureCoord);
	vec3 specularColour = ueGetMaterialSpecular(textureCoord);
	float shininess = ueGetMaterialShininess(textureCoord);
	
	vec3 normal = ueCalculateNormal(textureCoord);
	
	vec3 light = ueGetLighting(normal, ue_frag_position, ambientColour, diffuseColour.xyz, specularColour, shininess, ue_frag_pos_lightspace);

	ue_FragColour = vec4(light, diffuseColour.a);
}
