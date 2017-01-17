#include "Core.fs"

#map uniform Texture ue_tex

//The texture to display
uniform sampler2D ue_tex;

void main() {
	//Assign the colour
	ue_FragColour = texture(ue_tex, ue_frag_textureCoord);
}