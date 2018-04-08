#include "PBRLightingCore.fs"

out vec4 ue_FragColour;

void main() {
    vec2 textureCoord = ueCalculateTextureCoord();

	vec3 albedo = ueGetMaterialDiffuse(textureCoord).rgb;
	vec3 normal = ueCalculateNormal(textureCoord);
	float metalness = ueGetMaterialAmbient(textureCoord).r;
	float roughness = ueGetMaterialShininess(textureCoord);
	float ao = ueGetMaterialSpecular(textureCoord).r;

    normal = normalize(normal);

    vec3 colour = ueGetLightingPBR(normal, ue_frag_position, albedo, metalness, roughness, ao, ue_frag_pos_lightspace);

    colour = ueGammaCorrect(ueReinhardToneMapping(colour));

	ue_FragColour = vec4(colour, 1.0);
	//ue_FragColour = vec4(ue_frag_textureCoord, 0.0, 1.0);
}