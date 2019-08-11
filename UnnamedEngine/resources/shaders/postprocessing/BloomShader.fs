#include "PostProcess.fs"

#map uniform Texture1 ue_brightTexture

uniform sampler2D ue_brightTexture;

layout(location = 0) out vec4 ue_FragColour;
layout(location = 1) out vec4 ue_FragColourBright;

void main() {
	//Output the colour
	ue_FragColour = vec4(texture(ue_texture, ue_frag_textureCoord).rgb, 1.0);
	ue_FragColourBright = vec4(texture(ue_brightTexture, ue_frag_textureCoord).rgb, 1.0);
}