#include "Core.fs"

#map uniform Texture tex

//The texture to display
uniform sampler2D tex;

void main() {
	//Assign the colour
	FragColour = texture(tex, frag_textureCoord);
}