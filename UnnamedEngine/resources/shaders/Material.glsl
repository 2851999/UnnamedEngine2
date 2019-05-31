#map uniform Material_AmbientTexture ue_materialTextures.ambientTexture
#map uniform Material_DiffuseTexture ue_materialTextures.diffuseTexture
#map uniform Material_SpecularTexture ue_materialTextures.specularTexture
#map uniform Material_ShininessTexture ue_materialTextures.shininessTexture
#map uniform Material_NormalMap ue_materialTextures.normalMap
#map uniform Material_ParallaxMap ue_materialTextures.parallaxMap

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

/* The material textures structure */
struct UEMaterialTextures {
	sampler2D ambientTexture;
	sampler2D diffuseTexture;
	sampler2D specularTexture;
	sampler2D shininessTexture;
	sampler2D normalMap;
	sampler2D parallaxMap;
};

/* The material instance */
layout (std140, binding = 2) uniform UEMaterialData {
	UEMaterial ue_material;
};
uniform UEMaterialTextures ue_materialTextures;