#version 140

#map uniform LightSpaceMatrix mat
#map attribute Position position

uniform mat4 mat;

in vec3 position;

void main() {
	gl_Position = mat * vec4(position, 1.0);
}