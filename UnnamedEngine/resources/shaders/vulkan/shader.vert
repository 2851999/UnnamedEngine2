#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 1) out vec2 fragTexCoord;

layout (std140, binding = 0) uniform MatrixData {
	mat4 mvpMatrix;
};

//Note dvec3 is 64-bit so location of next value must be 2 higher than previous

void main() {
    gl_Position = mvpMatrix * vec4(inPosition, 0.0, 1.0);
	fragTexCoord = inTexCoord;
}