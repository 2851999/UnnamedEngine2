#include "Lighting.fs"

#define UE_LOCATION_GEOMETRY_POSITION 0
#define UE_LOCATION_GEOMETRY_NORMAL 1
#define UE_LOCATION_GEOMETRY_ALBEDO 2

layout(location = UE_LOCATION_GEOMETRY_POSITION) out vec4 ue_gPosition;
layout(location = UE_LOCATION_GEOMETRY_NORMAL) out vec4 ue_gNormal;
layout(location = UE_LOCATION_GEOMETRY_ALBEDO) out vec4 ue_gAlbedo;

void main() {
	vec2 textureCoord = ueCalculateTextureCoord();

	//vec3 ambientColour = ueGetMaterialAmbient(textureCoord);
	vec4 diffuseColour = ueGetMaterialDiffuse(textureCoord);
	//vec3 specularColour = ueGetMaterialSpecular(textureCoord);
	float shininess = ueGetMaterialShininess(textureCoord);
	
	vec3 normal = ueCalculateNormal(textureCoord);
	
	//vec3 light = ueGetLighting(normal, ue_frag_position, ambientColour, diffuseColour.xyz, specularColour, ue_material.shininess);

	//ue_FragColour = vec4(light, diffuseColour.a);

	ue_gPosition = vec4(ue_frag_position, 1.0); //Current FramebufferAttachment class assumes 4 components
	ue_gNormal = vec4(normal, shininess);
	ue_gAlbedo = diffuseColour;
}