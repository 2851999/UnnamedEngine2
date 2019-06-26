#version 140

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;

layout(location = 0) out vec2 frag_textureCoord;

void main() {
	frag_textureCoord = textureCoord;
	
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}