#include "Core.fs"

#map uniform Material_DiffuseColour ue_colour
#map uniform Material_DiffuseTexture ue_tex

//Colour and texture for the font
uniform vec4 ue_colour;
uniform sampler2D ue_tex;

void main() {
	//Assign the final colour
	ue_FragColour = vec4(1.0, 1.0, 1.0, texture2D(ue_tex, ue_frag_textureCoord).r) * ue_colour;
}