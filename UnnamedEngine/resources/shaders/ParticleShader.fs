#version 140

#map uniform Texture tex

uniform sampler2D tex;

in vec4 frag_colour;
in vec2 frag_textureCoord;

out vec4 FragColour;

void main() {
	FragColour = frag_colour * texture2D(tex, frag_textureCoord);
}