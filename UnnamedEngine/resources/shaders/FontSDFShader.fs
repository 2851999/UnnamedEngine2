#include "Core.fs"
#include "Material.glsl"

layout(location = 0) out vec4 ue_FragColour;

//SEE: https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf

void main() {
	//Input values
	float softEdges = 1.0;

	float SOFT_EDGE_MIN = 0.4;
	float SOFT_EDGE_MAX = 0.5;

	vec4 baseColour = ue_material.diffuseColour;
	float distAlphaMask = texture(ue_material_diffuseTexture, ue_frag_textureCoord).a;

	if (softEdges > 0.0)
		baseColour.a *= smoothstep(SOFT_EDGE_MIN, SOFT_EDGE_MAX, distAlphaMask);
	else
		baseColour.a = (distAlphaMask >= 0.5 ? 1.0 : 0.0);

	ue_FragColour = baseColour;
}