#include "PostProcess.fs"

#map uniform BloomTexture ue_bloomTexture

uniform sampler2D ue_bloomTexture;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Output the colour
	ue_FragColour = vec4(texture(ue_texture, ue_frag_textureCoord).rgb + texture(ue_bloomTexture, ue_frag_textureCoord).rgb, 1.0);
}