#include "Core.fs"

layout(set = 1, binding = 1) uniform sampler2D ue_texture;

layout(location = 3) in float frag_visibility;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	vec4 texColour = texture(ue_texture, ue_frag_textureCoord);
	ue_FragColour = vec4(texColour.rgb, texColour.a * frag_visibility);
}