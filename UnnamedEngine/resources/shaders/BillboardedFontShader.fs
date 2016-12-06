#include "Core.fs"

#map uniform Material_DiffuseColour colour
#map uniform Material_DiffuseTexture tex

//Colour and texture for the font
uniform vec4 colour;
uniform sampler2D tex;

void main() {
	//Assign the final colour
	FragColour = vec4(1.0, 1.0, 1.0, texture2D(tex, frag_textureCoord).r) * colour;
}