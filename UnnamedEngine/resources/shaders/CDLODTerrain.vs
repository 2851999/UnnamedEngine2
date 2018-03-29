#include "Core.vs"

#map uniform HeightMap ue_heightMap
#map uniform Translation ue_translation
#map uniform Scale ue_scale
#map uniform Range ue_range
#map uniform GridSize ue_gridSize
#map uniform HeightScale ue_heightScale
#map uniform Size ue_size

uniform sampler2D ue_heightMap;
uniform vec3 ue_translation;
uniform float ue_scale;
uniform float ue_range;
uniform vec2 ue_gridSize;
uniform float ue_heightScale;
uniform float ue_size;

out float ue_frag_height;

vec2 ueTerrainMorphVertex(vec2 gridPos, vec2 vertex, float morphK) {
	vec2 fracPart = fract(gridPos.xy * ue_gridSize.xy * 0.5) * 2.0 / ue_gridSize.xy;
	return vertex.xy - fracPart * ue_scale * morphK;
}

float ueTerrainGetHeight(vec2 pos) {
	return ((texture(ue_heightMap, (pos / ue_size) + vec2(0.5, 0.5)).r) - 0.5) * ue_heightScale;
}

vec3 ueTerrainCalculateNormal(vec2 pos) {
	vec3 normal;

	float offset = 1.0;
	vec3 off = vec3(offset, offset, 0.0);
	float hL = ueTerrainGetHeight(pos - off.xz);
	float hR = ueTerrainGetHeight(pos + off.xz);
	float hD = ueTerrainGetHeight(pos - off.zy);
	float hU = ueTerrainGetHeight(pos + off.zy);

	normal.x = hL - hR;
	normal.y = hD - hU;
	normal.z = 2.0 * offset;
	normal = normalize(normal);

	return normal;
}

vec3 ueTerrainCalculateWorldPosition(vec3 position, vec3 translation, float scale, float morphStart, float morphEnd) {
	vec3 worldPosition = scale * position + translation;
	worldPosition = worldPosition;

	float height = ueTerrainGetHeight(worldPosition.xz);
	worldPosition.y = height;
	float dist = distance(ue_cameraPosition, worldPosition);
	float nextLevelThreshold = ((ue_range - dist) / scale);
	float morphK = 1.0 - smoothstep(morphStart, morphEnd, nextLevelThreshold);
	worldPosition.xz = ueTerrainMorphVertex(position.xz, worldPosition.xz, morphK);
	height = ueTerrainGetHeight(worldPosition.xz);
	worldPosition.y = height;

	return worldPosition;
}

void ueTerrainAssignTerrainData() {
	float morphStart = 0.0;
	float morphEnd = 0.40;

	vec3 worldPosition = ueTerrainCalculateWorldPosition(ue_position, ue_translation, ue_scale, morphStart, morphEnd);
	
	ue_frag_height = (worldPosition.y / ue_heightScale) + 0.5;
	
	ue_frag_normal = ueTerrainCalculateNormal(worldPosition.xz);

	ue_frag_textureCoord = worldPosition.xz - ue_translation.xz;

	ue_frag_position = worldPosition;
}