
layout (std140, binding = 11) uniform UEShadowCubemapData {
    mat4 shadowMatrices[6];
	vec4 lightPos;
};