#include "Lighting.fs"

void main() {
	vec3 ambientColour = ueGetMaterialAmbient(ue_frag_textureCoord);
	vec4 diffuseColour = ueGetMaterialDiffuse(ue_frag_textureCoord);
	vec3 specularColour = ueGetMaterialSpecular(ue_frag_textureCoord);
	
	vec3 normal;
	
	if (ue_useNormalMap) {
		normal = normalize(ue_frag_tbnMatrix * ueGetMaterialNormal(ue_frag_textureCoord));
	} else {
		normal = normalize(ue_frag_normal);
	}
	
	vec3 light = ueGetLighting(normal, ambientColour, diffuseColour.xyz, specularColour);

	ue_FragColour = vec4(light, diffuseColour.a);
}