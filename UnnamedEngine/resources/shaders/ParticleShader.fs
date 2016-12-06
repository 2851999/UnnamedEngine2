#include "Core.fs"

#map uniform Texture tex

//The texture for the particles
uniform sampler2D tex;

//Data from the vertex shader
in vec4 frag_colour;

void main() {
	//Assign the colour
	FragColour = frag_colour * texture2D(tex, frag_textureCoord);
}