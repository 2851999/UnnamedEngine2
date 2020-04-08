#version 420
//Used for assigning UBO block locations - can remove

layout(std140, binding = 21) uniform UECoreData {
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
layout(std140, binding = 22) uniform UEMaterialData {
	UEMaterial ue_material;
};

/* The texture data */
layout(binding = 0) uniform sampler2D ue_material_ambientTexture;
layout(binding = 1) uniform sampler2D ue_material_diffuseTexture;
layout(binding = 2) uniform sampler2D ue_material_specularTexture;
layout(binding = 3) uniform sampler2D ue_material_shininessTexture;
layout(binding = 4) uniform sampler2D ue_material_normalMap;
layout(binding = 5) uniform sampler2D ue_material_parallaxMap;

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

layout(location = 0) out vec4 ue_FragColour;

void main() {
	ue_FragColour = ueGetMaterialDiffuse(ue_frag_textureCoord);
}
