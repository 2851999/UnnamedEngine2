#map uniform ProjectionViewMatrix ue_projectionViewMatrix
#map uniform Camera_Right ue_cameraRight
#map uniform Camera_Up ue_cameraUp

uniform mat4 ue_projectionViewMatrix;
uniform vec3 ue_cameraRight;
uniform vec3 ue_cameraUp;

vec4 ueGetBillboardPosition(vec3 cent, vec3 pos, vec2 size) {
	return ue_projectionViewMatrix * vec4(cent + ((ue_cameraRight * pos.x * size.x) + (ue_cameraUp * pos.y * size.y)), 1.0);
}