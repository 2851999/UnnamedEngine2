#include "Core.fs"
#include "Material.glsl"

out vec4 ue_FragColour;

void main() {
	ue_FragColour = vec4(1.0, 1.0, 1.0, texture(ue_materialTextures.diffuseTexture, ue_frag_textureCoord).r) * ue_material.diffuseColour;
}