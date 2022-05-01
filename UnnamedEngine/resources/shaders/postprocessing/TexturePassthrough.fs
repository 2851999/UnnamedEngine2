#include "PostProcess.fs"

layout(location = 0) out vec4 FragColour;

void main() {
	FragColour = vec4(texture(ue_texture, ue_frag_textureCoord).rgb, 1.0);
}