#include "Lighting.fs"

out vec4 ue_FragColour;

void main() {
	vec2 textureCoord = ueCalculateTextureCoord();

	vec3 ambientColour = ueGetMaterialAmbient(textureCoord);
	vec4 diffuseColour = ueGetMaterialDiffuse(textureCoord);
	vec3 specularColour = ueGetMaterialSpecular(textureCoord);
	
	vec3 normal = ueCalculateNormal(textureCoord);
	
	vec3 light = ueGetLighting(normal, ue_frag_position, ambientColour, diffuseColour.xyz, specularColour, ue_material.shininess, ue_frag_pos_lightspace);

	//light = ueGammaCorrect(ueExposureToneMap(1.0, light));
	light = (ueExposureToneMap(1.0, light));

	ue_FragColour = vec4(light, diffuseColour.a);
}