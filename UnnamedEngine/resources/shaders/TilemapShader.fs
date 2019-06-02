#include "Core.fs"

#map uniform Texture tex

uniform sampler2D tex;

in float frag_visibility;

out vec4 ue_FragColour;

void main() {
	vec4 texColour = texture(tex, ue_frag_textureCoord);
	ue_FragColour = vec4(texColour.rgb, texColour.a * frag_visibility);
}