#version 140

uniform sampler2D tex;

in vec2 frag_textureCoord;

out vec4 FragColour;

void main() {
	FragColour = vec4(1.0 - texture(tex, frag_textureCoord).xyz, 1.0);
}