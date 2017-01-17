#map uniform Material_AmbientColour ue_material.ambientColour
#map uniform Material_AmbientTexture ue_material.ambientTexture
#map uniform Material_DiffuseColour ue_material.diffuseColour
#map uniform Material_DiffuseTexture ue_material.diffuseTexture
#map uniform Material_SpecularColour ue_material.specularColour
#map uniform Material_SpecularTexture ue_material.specularTexture
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
	sampler2D diffuseTexture;
	sampler2D specularTexture;
	
	sampler2D normalMap;
	sampler2D parallaxMap;
	
	float parallaxScale;
	
	float shininess;
};

/* The material instance */
uniform UEMaterial ue_material;

/* Various methods to get colours */
vec3 ueGetMaterialAmbient(vec2 textureCoord) {
	return ue_material.ambientColour * texture(ue_material.ambientTexture, textureCoord).xyz;
}

vec4 ueGetMaterialDiffuse(vec2 textureCoord) {
	return ue_material.diffuseColour * texture(ue_material.diffuseTexture, textureCoord);
}

vec3 ueGetMaterialSpecular(vec2 textureCoord) {
	return ue_material.specularColour * texture(ue_material.specularTexture, textureCoord).xyz;
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

float ueGetMaterialShininess() {
	return ue_material.shininess;
}