#include "Core.fs"

#map uniform Material_DiffuseColour colour
#map uniform Material_DiffuseTexture tex

uniform vec4 colour;
uniform sampler2D tex;

void main() {
	//Assign the final colour
	FragColour = colour * texture(tex, frag_textureCoord);
}