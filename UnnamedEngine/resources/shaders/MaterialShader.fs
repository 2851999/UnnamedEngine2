#include "Core.fs"
#include "Material.fs"

void main() {
	ue_FragColour = ueGetMaterialDiffuse(ue_frag_textureCoord);
}