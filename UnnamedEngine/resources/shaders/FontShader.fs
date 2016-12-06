#include "Core.fs"

#map uniform Material_DiffuseColour ue_colour
#map uniform Material_DiffuseTexture ue_tex

uniform sampler2D ue_tex;
uniform vec4 ue_colour;

void main() {
	ue_FragColour = vec4(1.0, 1.0, 1.0, texture(ue_tex, ue_frag_textureCoord).r) * ue_colour;
}