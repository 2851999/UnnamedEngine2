#include "Lighting.fs"

void main() {
	vec2 textureCoord = ue_frag_textureCoord;
	
	if (ue_useParallaxMap) {
		textureCoord = ueGetMaterialParallax(ue_frag_textureCoord, normalize(ue_tangentViewPos - ue_tangentFragPos));
		
		if(textureCoord.x > 1.0 || textureCoord.y > 1.0 || textureCoord.x < 0.0 || textureCoord.y < 0.0)
			discard;
	}

	vec3 ambientColour = ueGetMaterialAmbient(textureCoord);
	vec4 diffuseColour = ueGetMaterialDiffuse(textureCoord);
	vec3 specularColour = ueGetMaterialSpecular(textureCoord);
	
	vec3 normal;
	
	if (ue_useNormalMap) {
		normal = normalize(ue_frag_tbnMatrix * ueGetMaterialNormal(textureCoord));
	} else {
		normal = normalize(ue_frag_normal);
	}
	
	vec3 light = ueGetLighting(normal, ambientColour, diffuseColour.xyz, specularColour);

	ue_FragColour = vec4(light, diffuseColour.a);
}