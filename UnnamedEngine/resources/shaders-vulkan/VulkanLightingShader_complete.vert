
#version 420
//Used for assigning UBO block locations - can remove

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


#define UE_LOCATION_POSITION 0
#define UE_LOCATION_TEXTURE_COORD 1
#define UE_LOCATION_NORMAL 2
#define UE_LOCATION_TANGENT 3
#define UE_LOCATION_BITANGENT 4
#define UE_LOCATION_BONE_IDS 5
#define UE_LOCATION_BONE_WEIGHTS 6

layout(location = UE_LOCATION_POSITION) in vec3 ue_position;
layout(location = UE_LOCATION_TEXTURE_COORD) in vec2 ue_textureCoord;
layout(location = UE_LOCATION_NORMAL) in vec3 ue_normal;
layout(location = UE_LOCATION_TANGENT) in vec3 ue_tangent;
layout(location = UE_LOCATION_BITANGENT) in vec3 ue_bitangent;

layout(location = 0) out vec3 ue_frag_position;
layout(location = 1) out vec2 ue_frag_textureCoord;
layout(location = 2) out vec3 ue_frag_normal;

void ueAssignPosition() {
	//Might want to change like CDLOD terrain
	ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
}

void ueAssignTextureCoord() {
	ue_frag_textureCoord = ue_textureCoord;
}

void ueCalculatePosition() {
	gl_Position = ue_mvpMatrix * vec4(ue_position, 1.0);
}

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

layout(std140, set = 3, binding = 25) uniform UELightBatchData {
	UELight ue_lights[MAX_LIGHTS];
	mat4 ue_lightSpaceMatrix[MAX_LIGHTS];
	
	vec4 ue_lightAmbient;
	int ue_numLights;
	
	bool ue_useEnvironmentMap;
};

layout(location = 7) out vec3 ue_tangentViewPos;
layout(location = 8) out vec3 ue_tangentFragPos;

layout(location = 9) out mat3 ue_frag_tbnMatrix;
layout(location = 13) out vec4 ue_frag_pos_lightspace[MAX_LIGHTS];

void ueAssignLightingData() {
	mat3 normalMatrix = mat3(ue_normalMatrix);
	ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
	ue_frag_normal = normalMatrix * ue_normal;
	
	for (int i = 0; i < ue_numLights; i++)
		ue_frag_pos_lightspace[i] = ue_lightSpaceMatrix[i] * vec4(ue_frag_position, 1.0);
	
	if (ue_material.hasNormalMap) {
		vec3 T;
		vec3 B;
		
		T = normalize(normalMatrix * ue_tangent);
		B = normalize(normalMatrix * ue_bitangent);
		vec3 N = normalize(ue_frag_normal);
	
		ue_frag_tbnMatrix = mat3(-T, B, N);
		
		if (ue_material.hasParallaxMap) {
			ue_tangentViewPos = transpose(ue_frag_tbnMatrix) * ue_cameraPosition.xyz;
			ue_tangentFragPos = transpose(ue_frag_tbnMatrix) * ue_frag_position;
		}
	}
	
	//Assign the vertex position
	ueCalculatePosition();
}

void main() {
	ueAssignTextureCoord();
	ueAssignLightingData();
}
