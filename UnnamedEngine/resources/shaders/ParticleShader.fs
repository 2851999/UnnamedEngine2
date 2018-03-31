#include "Core.fs"

#map uniform Texture ue_tex

//The texture for the particles
uniform sampler2D ue_tex;

//Data from the vertex shader
in vec4 ue_frag_colour;

out vec4 ue_FragColour;

void main() {
	//Assign the colour
	ue_FragColour = ue_frag_colour * texture2D(ue_tex, ue_frag_textureCoord);
}