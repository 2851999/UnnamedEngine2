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

#ifdef UE_SKINNING
const int UE_MAX_BONES = 90;

layout(std140, set = 2, binding = 24) uniform UESkinningData {
	mat4 ue_bones[UE_MAX_BONES];
	bool ue_useSkinning;
};

layout(location = UE_LOCATION_BONE_IDS) in ivec4 ue_boneIDs;
layout(location = UE_LOCATION_BONE_WEIGHTS) in vec4 ue_boneWeights;

mat4 ueGetBoneTransform() {
	mat4 boneTransform = ue_bones[ue_boneIDs[0]] * ue_boneWeights[0];
	boneTransform += ue_bones[ue_boneIDs[1]] * ue_boneWeights[1];
	boneTransform += ue_bones[ue_boneIDs[2]] * ue_boneWeights[2];
	boneTransform += ue_bones[ue_boneIDs[3]] * ue_boneWeights[3];
	
	return boneTransform;
}
#endif

#ifndef UE_GEOMETRY_ONLY
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
#endif

layout(location = 7) out vec3 ue_tangentViewPos;
layout(location = 8) out vec3 ue_tangentFragPos;

layout(location = 9) out mat3 ue_frag_tbnMatrix;
#ifndef UE_GEOMETRY_ONLY
layout(location = 13) out vec4 ue_frag_pos_lightspace[MAX_LIGHTS];
#endif

#ifdef UE_SKINNING
void ueAssignLightingData() {
	mat4 boneTransform;
	mat3 normalMatrix = mat3(ue_normalMatrix);
	if (ue_useSkinning) {
		boneTransform = ueGetBoneTransform();
		
		ue_frag_position = vec3(ue_modelMatrix * boneTransform * vec4(ue_position, 1.0));
		
		ue_frag_normal = normalMatrix * vec3(boneTransform * vec4(ue_normal, 0.0));
	} else {
		ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
		ue_frag_normal = normalMatrix * ue_normal;
	}
#ifndef UE_GEOMETRY_ONLY
	for (int i = 0; i < ue_numLights; i++)
		ue_frag_pos_lightspace[i] = ue_lightSpaceMatrix[i] * vec4(ue_frag_position, 1.0);
#endif
	
	if (ue_material.hasNormalMap) {
		vec3 T;
		vec3 B;
		
		if (ue_useSkinning) {
			T = normalize(normalMatrix * vec3(boneTransform * vec4(ue_tangent, 0.0)));
			B = normalize(normalMatrix * vec3(boneTransform * vec4(ue_bitangent, 0.0)));
		} else {
			T = normalize(normalMatrix * ue_tangent);
			B = normalize(normalMatrix * ue_bitangent);
		}
		vec3 N = normalize(ue_frag_normal);
	
		ue_frag_tbnMatrix = mat3(-T, B, N);
		
		if (ue_material.hasParallaxMap) {
			ue_tangentViewPos = transpose(ue_frag_tbnMatrix) * ue_cameraPosition.xyz;
			ue_tangentFragPos = transpose(ue_frag_tbnMatrix) * ue_frag_position;
		}
	}
	
	//Assign the vertex position
	if (ue_useSkinning) {
		gl_Position = ue_mvpMatrix * boneTransform * vec4(ue_position, 1.0);
	} else {
		ueCalculatePosition();
	}
}
#else
void ueAssignLightingData() {
	mat3 normalMatrix = mat3(ue_normalMatrix);
	ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
	ue_frag_normal = normalMatrix * ue_normal;
	
#ifndef UE_GEOMETRY_ONLY
	for (int i = 0; i < ue_numLights; i++)
		ue_frag_pos_lightspace[i] = ue_lightSpaceMatrix[i] * vec4(ue_frag_position, 1.0);
#endif
	
	if (ue_material.hasNormalMap) {
		vec3 T = normalize(normalMatrix * ue_tangent);
		vec3 B = normalize(normalMatrix * ue_bitangent);
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
#endif

//Assign these as constants for now
#define UE_TERRAIN_NORMAL_LOOK_UP_OFFSET 1.0
#define UE_TERRAIN_VERTEX_MORPH_START 0.0
#define UE_TERRAIN_VERTEX_MORPH_END 0.40

layout(set = 4, binding = 6) uniform sampler2D ue_heightMap;

/* The terrain data */
layout(std140, set = 4, binding = 26) uniform UETerrainData {
	vec4 ue_translation;
	vec2 ue_gridSize;
	float ue_scale;
	float ue_range;
	float ue_heightScale;
	float ue_size;
};

layout(location = 3) out float ue_frag_height;

vec2 ueTerrainMorphVertex(vec2 gridPos, vec2 vertex, float morphK) {
	vec2 fracPart = fract(gridPos.xy * ue_gridSize.xy * 0.5) * 2.0 / ue_gridSize.xy;
	return vertex.xy - fracPart * ue_scale * morphK;
}

float ueTerrainGetHeight(vec2 pos) {
	return ((texture(ue_heightMap, (pos / ue_size) + vec2(0.5, 0.5)).r) - 0.5) * ue_heightScale;
}

vec3 ueTerrainCalculateNormal(vec2 pos) {
	vec3 normal;

	vec3 off = vec3(UE_TERRAIN_NORMAL_LOOK_UP_OFFSET, UE_TERRAIN_NORMAL_LOOK_UP_OFFSET, 0.0);
	float hL = ueTerrainGetHeight(pos - off.xz);
	float hR = ueTerrainGetHeight(pos + off.xz);
	float hD = ueTerrainGetHeight(pos - off.zy);
	float hU = ueTerrainGetHeight(pos + off.zy);

	normal.x = hL - hR;
	normal.y = 2.0 * UE_TERRAIN_NORMAL_LOOK_UP_OFFSET;
	normal.z = hD - hU;
	normal = normalize(normal);

	return normal;
}

vec3 ueTerrainCalculateWorldPosition(vec3 position, vec3 translation, float scale, float morphStart, float morphEnd) {
	vec3 worldPosition = scale * position + translation;
	worldPosition = worldPosition;

	float height = ueTerrainGetHeight(worldPosition.xz);
	worldPosition.y = height;
	float dist = distance(ue_cameraPosition.xyz, worldPosition);
	float nextLevelThreshold = ((ue_range - dist) / scale);
	float morphK = 1.0 - smoothstep(morphStart, morphEnd, nextLevelThreshold);
	worldPosition.xz = ueTerrainMorphVertex(position.xz, worldPosition.xz, morphK);
	height = ueTerrainGetHeight(worldPosition.xz);
	worldPosition.y = height;

	return worldPosition;
}

void ueTerrainAssignTerrainData() {
	vec3 worldPosition = ueTerrainCalculateWorldPosition(ue_position, ue_translation.xyz, ue_scale, UE_TERRAIN_VERTEX_MORPH_START, UE_TERRAIN_VERTEX_MORPH_END);
	
	ue_frag_height = (worldPosition.y / ue_heightScale) + 0.5;
	
	ue_frag_normal = ueTerrainCalculateNormal(worldPosition.xz);

	ue_frag_textureCoord = worldPosition.xz - ue_translation.xz;

	ue_frag_position = worldPosition;
}

void main() {
	ueTerrainAssignTerrainData();

	for (int i = 0; i < ue_numLights; i++)
		ue_frag_pos_lightspace[i] = ue_lightSpaceMatrix[i] * vec4(ue_frag_position, 1.0);
	
	gl_Position = ue_mvpMatrix * vec4(ue_frag_position, 1.0);
}
