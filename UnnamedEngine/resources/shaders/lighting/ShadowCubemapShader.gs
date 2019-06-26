#version 420

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out; //1 triangle in, 6 out (18 vertices)

#include "ShadowCubemapShader.glsl"

layout(location = 0) out vec4 FragPos;

void main() {
	for (int face = 0; face < 6; ++face) {
		gl_Layer = face;
		
		for (int i = 0; i < 3; ++i) {
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}