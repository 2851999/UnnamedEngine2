#version 140

uniform sampler2D tex;

layout(location = 0) in vec2 frag_textureCoord;

layout(location = 0) out vec4 FragColour;

void main() {
	FragColour = vec4(texture(tex, frag_textureCoord).rgb, 1.0);
}