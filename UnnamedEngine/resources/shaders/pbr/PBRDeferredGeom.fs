#include "../lighting/Lighting.fs"

#define UE_LOCATION_GEOMETRY_POSITION 0
#define UE_LOCATION_GEOMETRY_NORMAL 1
#define UE_LOCATION_GEOMETRY_ALBEDO 2
#define UE_LOCATION_GEOMETRY_METALNESS_AO 3

layout (location = UE_LOCATION_GEOMETRY_POSITION) out vec3 ue_gPosition;
layout (location = UE_LOCATION_GEOMETRY_NORMAL) out vec4 ue_gNormal;
layout (location = UE_LOCATION_GEOMETRY_ALBEDO) out vec4 ue_gAlbedo;
layout (location = UE_LOCATION_GEOMETRY_METALNESS_AO) out vec3 ue_gMetalnessAO;

void main() {
	vec2 textureCoord = ueCalculateTextureCoord();

	vec4 albedo = ueGetMaterialDiffuse(textureCoord);

	float metalness = ueGetMaterialAmbient(textureCoord).r;
	float roughness = ueGetMaterialShininess(textureCoord);
	float ao = ueGetMaterialSpecular(textureCoord).r;
	
	vec3 normal = ueCalculateNormal(textureCoord);

	ue_gPosition = ue_frag_position;
	ue_gNormal = vec4(normal, roughness);
	ue_gAlbedo = albedo;
	ue_gMetalnessAO = vec3(metalness, ao, 0.0);
}