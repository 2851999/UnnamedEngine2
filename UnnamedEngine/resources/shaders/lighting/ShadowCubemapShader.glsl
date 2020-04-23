
layout(std140, set = 3, binding = 12) uniform UEShadowCubemapData {
    mat4 shadowMatrices[6];
	vec4 lightPos;
};