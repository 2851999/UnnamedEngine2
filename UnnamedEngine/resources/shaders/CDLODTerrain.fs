#version 330

in float frag_morph;

out vec4 FragColour;

void main() {
	//FragColour = vec4(1.0, 1.0, 1.0, 1.0);
	FragColour = vec4(frag_morph, frag_morph, frag_morph, 1.0);
}