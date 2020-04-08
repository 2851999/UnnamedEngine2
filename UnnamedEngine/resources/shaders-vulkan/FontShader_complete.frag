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

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Assign the final colour
	ue_FragColour = vec4(1.0, 1.0, 1.0, texture(ue_material_diffuseTexture, ue_frag_textureCoord).r) * ue_material.diffuseColour;
}
