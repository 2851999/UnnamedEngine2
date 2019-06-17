#include "Core.fs"
#include "Material.glsl"

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Assign the final colour
	ue_FragColour = vec4(1.0, 1.0, 1.0, texture(ue_material_diffuseTexture, ue_frag_textureCoord).r) * ue_material.diffuseColour;
}