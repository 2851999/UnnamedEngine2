#include "Core.fs"

//The texture for the particles
layout(set = 1, binding = 1) uniform sampler2D ue_texture;

//Data from the vertex shader
layout(location = 3) in vec4 ue_frag_colour;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Assign the colour
	ue_FragColour = ue_frag_colour * texture(ue_texture, ue_frag_textureCoord);
}