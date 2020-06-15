
layout(std140, set = 3, binding = 11) uniform UEBillboardData {
	mat4 ue_projectionViewMatrix;
	vec4 ue_cameraRight;
	vec4 ue_cameraUp;
	vec4 ue_billboardCentre;
	vec2 ue_billboardSize;
};

vec4 ueGetBillboardPosition(vec3 cent, vec3 pos, vec2 size) {
	return ue_projectionViewMatrix * vec4(cent + ((ue_cameraRight.xyz * pos.x * size.x) + (ue_cameraUp.xyz * pos.y * size.y)), 1.0);
}