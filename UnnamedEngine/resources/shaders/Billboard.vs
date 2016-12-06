#map uniform ProjectionViewMatrix projectionViewMatrix
#map uniform Camera_Right cameraRight
#map uniform Camera_Up cameraUp

uniform mat4 projectionViewMatrix;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

vec4 ue_getBillboardPosition(vec3 cent, vec3 pos, vec2 size) {
	return projectionViewMatrix * vec4(cent + ((cameraRight * pos.x * size.x) + (cameraUp * pos.y * size.y)), 1.0);
}