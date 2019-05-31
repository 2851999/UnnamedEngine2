#include "Material.glsl"

/* Various methods to get colours */
vec3 ueGetMaterialAmbient(vec2 textureCoord) {
	vec3 ambientColour = vec3(ue_material.ambientColour);
	if (ue_material.hasAmbientTexture)
		ambientColour *= texture(ue_materialTextures.ambientTexture, textureCoord).rgb;
	return ambientColour;
}

vec4 ueGetMaterialDiffuse(vec2 textureCoord) {
	vec4 diffuseColour = ue_material.diffuseColour;
	if (ue_material.hasDiffuseTexture) {
		// vec4 tex = texture(ue_material.diffuseTexture, textureCoord);
		// if (ue_material.diffuseTextureSRGB)
		// 	tex = pow(tex, vec4(2.2));
		// diffuseColour *= tex;
		diffuseColour *= texture(ue_materialTextures.diffuseTexture, textureCoord);
	}
	return diffuseColour;
}

vec3 ueGetMaterialSpecular(vec2 textureCoord) {
	vec3 specularColour = vec3(ue_material.specularColour);
	if (ue_material.hasSpecularTexture)
		specularColour *= texture(ue_materialTextures.specularTexture, textureCoord).rgb;
	return specularColour;
}

float ueGetMaterialShininess(vec2 textureCoord) {
	if (ue_material.hasShininessTexture)
		return texture(ue_materialTextures.shininessTexture, textureCoord).r;
	else
		return ue_material.shininess;
}

vec3 ueGetMaterialNormal(vec2 textureCoord) {
	vec3 normal = texture(ue_materialTextures.normalMap, textureCoord).rgb;
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
	float currentDepthMapValue = texture(ue_materialTextures.parallaxMap, currentTexCoords).r;
	
	while(currentLayerDepth < currentDepthMapValue) {
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(ue_materialTextures.parallaxMap, currentTexCoords).r;  
		currentLayerDepth += layerSize;
	}
	
	//Parallax occlusion mapping interpolation
	
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	
	float nextDepth = currentDepthMapValue - currentLayerDepth;
	float lastDepth = texture(ue_materialTextures.parallaxMap, prevTexCoords).r - currentLayerDepth + layerSize;
	float weight = nextDepth / (nextDepth - lastDepth);
	
	return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
}