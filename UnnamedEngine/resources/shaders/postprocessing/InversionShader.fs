#include "PostProcess.fs"

out vec4 FragColour;

void main() {
	FragColour = vec4(1.0 - texture(ue_texture, ue_frag_textureCoord).xyz, 1.0);
}