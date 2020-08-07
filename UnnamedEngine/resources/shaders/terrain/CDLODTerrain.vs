#include "../lighting/Lighting.vs"

//Assign these as constants for now
#define UE_TERRAIN_NORMAL_LOOK_UP_OFFSET 1.0
#define UE_TERRAIN_VERTEX_MORPH_START 0.0
#define UE_TERRAIN_VERTEX_MORPH_END 0.40

layout(set = 4, binding = 6) uniform sampler2D ue_heightMap;

/* The terrain data */
layout(std140, set = 4, binding = 6) uniform UETerrainData {
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