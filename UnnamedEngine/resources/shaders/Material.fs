#map uniform Material_AmbientColour ue_material.ambientColour
#map uniform Material_AmbientTexture ue_material.ambientTexture
#map uniform Material_HasAmbientTexture ue_material.hasAmbientTexture
#map uniform Material_DiffuseColour ue_material.diffuseColour
#map uniform Material_DiffuseTexture ue_material.diffuseTexture
#map uniform Material_HasDiffuseTexture ue_material.hasDiffuseTexture
#map uniform Material_DiffuseTextureSRGB ue_material.diffuseTextureSRGB
#map uniform Material_SpecularColour ue_material.specularColour
#map uniform Material_SpecularTexture ue_material.specularTexture
#map uniform Material_HasSpecularTexture ue_material.hasSpecularTexture
#map uniform Material_ShininessTexture ue_material.shininessTexture
#map uniform Material_HasShininessTexture ue_material.hasShininessTexture
#map uniform Material_NormalMap ue_material.normalMap
#map uniform Material_ParallaxMap ue_material.parallaxMap
#map uniform Material_ParallaxScale ue_material.parallaxScale
#map uniform Material_Shininess ue_material.shininess

/* The material structure */
struct UEMaterial {
	vec3 ambientColour;
	vec4 diffuseColour;
	vec3 specularColour;
	
	sampler2D ambientTexture;
	bool hasAmbientTexture;

	sampler2D diffuseTexture;
	bool hasDiffuseTexture;
	bool diffuseTextureSRGB;

	sampler2D specularTexture;
	bool hasSpecularTexture;

	sampler2D shininessTexture;
	bool hasShininessTexture;
	
	sampler2D normalMap;
	sampler2D parallaxMap;
	
	float parallaxScale;
	
	float shininess;
};

/* The material instance */
uniform UEMaterial ue_material;

/* Various methods to get colours */
vec3 ueGetMaterialAmbient(vec2 textureCoord) {
	vec3 ambientColour = ue_material.ambientColour;
	if (ue_material.hasAmbientTexture)
		ambientColour *= texture(ue_material.ambientTexture, textureCoord).rgb;
	return ambientColour;
}

vec4 ueGetMaterialDiffuse(vec2 textureCoord) {
	vec4 diffuseColour = ue_material.diffuseColour;
	if (ue_material.hasDiffuseTexture) {
		// vec4 tex = texture(ue_material.diffuseTexture, textureCoord);
		// if (ue_material.diffuseTextureSRGB)
		// 	tex = pow(tex, vec4(2.2));
		// diffuseColour *= tex;
		diffuseColour *= texture(ue_material.diffuseTexture, textureCoord);
	}
	return diffuseColour;
}

vec3 ueGetMaterialSpecular(vec2 textureCoord) {
	vec3 specularColour = ue_material.specularColour;
	if (ue_material.hasSpecularTexture)
		specularColour *= texture(ue_material.specularTexture, textureCoord).rgb;
	return specularColour;
}

float ueGetMaterialShininess(vec2 textureCoord) {
	if (ue_material.hasShininessTexture)
		return texture(ue_material.shininessTexture, textureCoord).r;
	else
		return ue_material.shininess;
}

vec3 ueGetMaterialNormal(vec2 textureCoord) {
	vec3 normal = texture(ue_material.normalMap, textureCoord).rgb;
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
	float currentDepthMapValue = texture(ue_material.parallaxMap, currentTexCoords).r;
	
	while(currentLayerDepth < currentDepthMapValue) {
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(ue_material.parallaxMap, currentTexCoords).r;  
		currentLayerDepth += layerSize;
	}
	
	//Parallax occlusion mapping interpolation
	
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	
	float nextDepth = currentDepthMapValue - currentLayerDepth;
	float lastDepth = texture(ue_material.parallaxMap, prevTexCoords).r - currentLayerDepth + layerSize;
	float weight = nextDepth / (nextDepth - lastDepth);
	
	return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
}