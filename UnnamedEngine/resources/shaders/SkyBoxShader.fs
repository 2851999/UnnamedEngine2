#include "Core.fs"

#map uniform Material_DiffuseTexture ue_tex

layout(set = 1, binding = 1) uniform samplerCube ue_tex;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	ue_FragColour = texture(ue_tex, ue_frag_position);
}