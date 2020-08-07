#include "PostProcess.fs"

layout(set = 1, binding = 0) uniform sampler2D ue_bloomTexture;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Output the colour
	ue_FragColour = vec4(texture(ue_texture, ue_frag_textureCoord).rgb + texture(ue_bloomTexture, ue_frag_textureCoord).rgb, 1.0);
}