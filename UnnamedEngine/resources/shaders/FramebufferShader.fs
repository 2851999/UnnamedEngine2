#include "Core.fs"

#map uniform Texture ue_tex

//The texture to display
layout(set = 1, binding = 1) uniform sampler2D ue_tex;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Assign the colour
	ue_FragColour = texture(ue_tex, ue_frag_textureCoord);
}