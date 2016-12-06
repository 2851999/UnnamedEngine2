#include "Lighting.fs"

void main() {
	vec3 ambientColour = ue_getMaterialAmbient(frag_textureCoord);
	vec4 diffuseColour = ue_getMaterialDiffuse(frag_textureCoord);
	vec3 specularColour = ue_getMaterialSpecular(frag_textureCoord);
	
	vec3 normal;
	
	if (useNormalMap) {
		normal = normalize(frag_tbnMatrix * ue_getMaterialNormal(frag_textureCoord));
	} else {
		normal = normalize(frag_normal);
	}
	
	vec3 light = ue_getLighting(normal, ambientColour, diffuseColour.xyz, specularColour);

	FragColour = vec4(light, diffuseColour.a);
}