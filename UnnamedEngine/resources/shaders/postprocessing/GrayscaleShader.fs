#version 140

uniform sampler2D tex;

in vec2 frag_textureCoord;

out vec4 FragColour;

void main() {
	FragColour = texture(tex, frag_textureCoord);
	float average = 0.2126 * FragColour.r + 0.7152 * FragColour.g + 0.0722 * FragColour.b;
	FragColour = vec4(average, average, average, 1.0);
}