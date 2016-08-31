#version 140

#map uniform Texture tex

uniform sampler2D tex;

in vec2 frag_textureCoord;

out vec4 FragColour;

void main() {
	FragColour = texture(tex, frag_textureCoord);
}