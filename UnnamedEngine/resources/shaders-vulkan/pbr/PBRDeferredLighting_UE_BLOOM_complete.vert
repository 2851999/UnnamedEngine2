#version 420
#define UE_BLOOM


layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;

layout(location = 1) out vec2 ue_frag_textureCoord;

void main() {
	ue_frag_textureCoord = textureCoord;
	
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
