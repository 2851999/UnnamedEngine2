#include "Core.fs"
#include "Material.fs"

void main() {
	FragColour = ue_getMaterialDiffuse(frag_textureCoord);
}