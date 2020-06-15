#version 420

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

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Assign the final colour
	ue_FragColour = vec4(1.0, 1.0, 1.0, texture(ue_material_diffuseTexture, ue_frag_textureCoord).r) * ue_material.diffuseColour;
}
