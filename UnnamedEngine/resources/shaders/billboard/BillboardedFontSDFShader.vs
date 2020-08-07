#include "../Core.vs"

//Would be better to #include "Billboard.vs" but have to differentiate between Font and Text, and SDF text rendering already
//uses set 3, so for Vulkan to work, need to change to set 4 for the billboard data

layout(std140, set = 4, binding = 11) uniform UEBillboardData {
	mat4 ue_projectionViewMatrix;
	vec4 ue_cameraRight;
	vec4 ue_cameraUp;
	vec4 ue_billboardCentre;
	vec2 ue_billboardSize;
};

vec4 ueGetBillboardPosition(vec3 cent, vec3 pos, vec2 size) {
	return ue_projectionViewMatrix * vec4(cent + ((ue_cameraRight.xyz * pos.x * size.x) + (ue_cameraUp.xyz * pos.y * size.y)), 1.0);
}

void main() {
	ueAssignTextureCoord();
	
	gl_Position = ueGetBillboardPosition(ue_billboardCentre.xyz, ue_position, ue_billboardSize);
}