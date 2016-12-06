#include "Core.fs"

#map uniform Material_DiffuseTexture ue_tex

uniform samplerCube ue_tex;

void main() {
	ue_FragColour = texture(ue_tex, ue_frag_position);
}