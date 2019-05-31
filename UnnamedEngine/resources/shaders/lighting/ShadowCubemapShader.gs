#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out; //1 triangle in, 6 out (18 vertices)

#map uniform ShadowMatrices[0] shadowMatrices[0]
#map uniform ShadowMatrices[1] shadowMatrices[1]
#map uniform ShadowMatrices[2] shadowMatrices[2]
#map uniform ShadowMatrices[3] shadowMatrices[3]
#map uniform ShadowMatrices[4] shadowMatrices[4]
#map uniform ShadowMatrices[5] shadowMatrices[5]

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

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