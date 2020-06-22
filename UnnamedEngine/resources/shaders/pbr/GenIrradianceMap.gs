#version 420

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out; //1 triangle in, 6 out (18 vertices)

layout(std140, set = 0, binding = 0) uniform UEEnvMapGenData {
	mat4 projectionViewMatrices[6];
};

layout(location = 0) out vec4 localPos;

void main() {
	for (int face = 0; face < 6; ++face) {
		gl_Layer = face;
		
		for (int i = 0; i < 3; ++i) {
			localPos = gl_in[i].gl_Position;
			gl_Position = projectionViewMatrices[face] * localPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}