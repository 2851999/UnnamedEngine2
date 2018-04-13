#include "Core.fs"
#include "Material.fs"

out vec4 ue_FragColour;

void main() {
	ue_FragColour = ueGetMaterialDiffuse(ue_frag_textureCoord);
}