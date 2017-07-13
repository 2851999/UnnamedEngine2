#version 330

in float frag_colour;

out vec4 FragColour;

void main() {
	//FragColour = vec4(1.0, 1.0, 1.0, 1.0);
	FragColour = vec4(frag_colour, frag_colour, frag_colour, 1.0);
/*	vec3 colour = vec3(frag_colour, 1.0, frag_colour);
	if (frag_colour < 0.1) {
		colour = vec3(frag_colour, frag_colour, 1.0);
	}
	FragColour = vec4(colour, 1.0); */
}