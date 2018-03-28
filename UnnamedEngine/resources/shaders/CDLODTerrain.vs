#version 330

#map uniform ProjectionMatrix projectionMatrix
#map uniform ViewMatrix viewMatrix
#map uniform HeightMap heightMap
#map uniform CameraPosition cameraPosition
#map uniform Translation translation
#map uniform Scale scale
#map uniform Range range
#map uniform GridSize gridSize
#map uniform HeightScale heightScale
#map uniform Size size
#map attribute Position position

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform sampler2D heightMap;
uniform vec3 cameraPosition;
uniform vec3 translation;
uniform float scale;
uniform float range;
uniform vec2 gridSize;
uniform float heightScale;
uniform float size;

in vec3 position;

out vec3 ue_frag_position;
out vec3 frag_worldPosition;
out float frag_distance;
out vec2 frag_screenPos;
out vec2 ue_frag_textureCoord;
out float frag_height;
out vec3 ue_frag_normal;

vec2 morphVertex(vec2 gridPos, vec2 vertex, float morphK) {
	vec2 fracPart = fract(gridPos.xy * gridSize.xy * 0.5) * 2.0 / gridSize.xy;
	return vertex.xy - fracPart * scale * morphK;
}

float getHeight(vec2 pos) {
	return ((texture(heightMap, (pos / size) + vec2(0.5, 0.5)).r) - 0.5) * heightScale;
}

void main() {
	float morphStart = 0.0;
	float morphEnd = 0.25;
	
	frag_worldPosition = scale * position + translation;
	float height = getHeight(frag_worldPosition.xz);
	frag_worldPosition.y = height;
	float dist = distance(cameraPosition, frag_worldPosition);
	float nextLevelThreshold = ((range - dist) / scale);
	float morphK = 1.0 - smoothstep(morphStart, morphEnd, nextLevelThreshold);
	//frag_colour = range / dist;
	//frag_colour = morphK;
	frag_worldPosition.xz = morphVertex(position.xz, frag_worldPosition.xz, morphK);
	height = getHeight(frag_worldPosition.xz);
	frag_worldPosition.y = height;
	
	frag_height = (height / heightScale) + 0.5;
	
	//TEST
	vec3 N;
	float offset = 1.0;
	vec3 off = vec3(offset, offset, 0.0);
	float hL = getHeight(frag_worldPosition.xz - off.xz);
	float hR = getHeight(frag_worldPosition.xz + off.xz);
	float hD = getHeight(frag_worldPosition.xz - off.zy);
	float hU = getHeight(frag_worldPosition.xz + off.zy);

	N.x = hL - hR;
	N.y = hD - hU;
	N.z = 2.0 * offset;
	N = normalize(N);
	
	ue_frag_normal = N;

	ue_frag_textureCoord = (frag_worldPosition.xz - translation.xz);
	
	ue_frag_position = vec3(viewMatrix * vec4(frag_worldPosition, 1.0));
	frag_distance = dist;
	gl_Position = projectionMatrix * vec4(ue_frag_position, 1.0);
	frag_screenPos = gl_Position.xy;
}