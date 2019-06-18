#map uniform Material_AmbientTexture ue_material_ambientTexture
#map uniform Material_DiffuseTexture ue_material_diffuseTexture
#map uniform Material_SpecularTexture ue_material_specularTexture
#map uniform Material_ShininessTexture ue_material_shininessTexture
#map uniform Material_NormalMap ue_material_normalMap
#map uniform Material_ParallaxMap ue_material_parallaxMap

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
layout (std140, binding = 2) uniform UEMaterialData {
	UEMaterial ue_material;
};

/* The texture data */
layout(binding = 0) uniform sampler2D ue_material_ambientTexture;
layout(binding = 1) uniform sampler2D ue_material_diffuseTexture;
layout(binding = 2) uniform sampler2D ue_material_specularTexture;
layout(binding = 3) uniform sampler2D ue_material_shininessTexture;
layout(binding = 4) uniform sampler2D ue_material_normalMap;
layout(binding = 5) uniform sampler2D ue_material_parallaxMap;