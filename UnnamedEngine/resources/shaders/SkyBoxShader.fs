#include "Core.fs"

#map uniform Material_DiffuseTexture ue_tex

uniform samplerCube ue_tex;

out vec4 ue_FragColour;

void main() {
	ue_FragColour = texture(ue_tex, ue_frag_position);
}