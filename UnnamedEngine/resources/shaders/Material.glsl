/* The material structure */
struct UEMaterial {
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;
	vec4 emissiveColour;
	
	bool hasAmbientTexture;
	bool hasDiffuseTexture;
	bool diffuseTextureSRGB;
	bool hasSpecularTexture;
	bool hasShininessTexture;
	bool hasNormalMap;
	bool hasParallaxMap;
	bool hasEmissiveTexture;
	
	float parallaxScale;
	float shininess;
};

/* The material data */
layout(std140, set = 1, binding = 3) uniform UEMaterialData {
	UEMaterial ue_material;
};

/* The texture data */
layout(set = 1, binding = 0) uniform sampler2D ue_material_ambientTexture;
layout(set = 1, binding = 1) uniform sampler2D ue_material_diffuseTexture;
layout(set = 1, binding = 2) uniform sampler2D ue_material_specularTexture;
layout(set = 1, binding = 3) uniform sampler2D ue_material_shininessTexture;
layout(set = 1, binding = 4) uniform sampler2D ue_material_normalMap;
layout(set = 1, binding = 5) uniform sampler2D ue_material_parallaxMap;
layout(set = 1, binding = 6) uniform sampler2D ue_material_emissiveTexture;