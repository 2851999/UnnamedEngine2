#version 420

#map attribute Position aPos

layout (location = 0) in vec3 aPos;

layout(location = 0) out vec3 localPos;

layout (std140, binding = 8) uniform UEEnvMapGenData {
	mat4 projection;
	mat4 view;
};

void main() {
	localPos = aPos;
	gl_Position = projection * view * vec4(localPos, 1.0);
}