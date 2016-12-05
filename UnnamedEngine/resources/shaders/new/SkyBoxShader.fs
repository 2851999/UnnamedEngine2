#include "Core.fs"

#map uniform Material_DiffuseTexture tex

uniform samplerCube tex;

void main() {
	FragColour = texture(tex, frag_position);
}