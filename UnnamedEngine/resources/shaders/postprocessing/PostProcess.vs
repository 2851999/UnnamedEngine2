#version 420

#map attribute Position ue_position
#map attribute TextureCoordinate ue_textureCoord

layout(location = 0) in vec3 ue_position;
layout(location = 1) in vec2 ue_textureCoord;

layout(location = 0) out vec2 ue_frag_textureCoord;

void main() {
	ue_frag_textureCoord = ue_textureCoord;
	
	gl_Position = vec4(ue_position.x, ue_position.y, 0.0, 1.0);
}