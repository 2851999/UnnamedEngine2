#include "../Core.fs"

#map uniform Material_DiffuseColour ue_colour
#map uniform Material_DiffuseTexture ue_tex

uniform vec4 ue_colour;
uniform sampler2D ue_tex;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Assign the final colour
	ue_FragColour = ue_colour * texture(ue_tex, ue_frag_textureCoord);
}