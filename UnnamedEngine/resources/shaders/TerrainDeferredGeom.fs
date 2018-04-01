#include "Lighting.fs"
#include "CDLODTerrain.fs"

#define UE_LOCATION_GEOMETRY_POSITION 0
#define UE_LOCATION_GEOMETRY_NORMAL 1
#define UE_LOCATION_GEOMETRY_ALBEDO 2
#define UE_LOCATION_GEOMETRY_SPECULAR 3

layout (location = UE_LOCATION_GEOMETRY_POSITION) out vec3 ue_gPosition;
layout (location = UE_LOCATION_GEOMETRY_NORMAL) out vec4 ue_gNormal;
layout (location = UE_LOCATION_GEOMETRY_ALBEDO) out vec4 ue_gAlbedo;

void main() {
	vec2 textureCoord = ueCalculateTextureCoord();
	vec4 diffuseColour = ueGetMaterialDiffuse(textureCoord);
	vec3 normal = ueCalculateNormal(textureCoord);

	ue_gPosition = ue_frag_position;
	ue_gNormal = vec4(normal, ue_material.shininess);
	ue_gAlbedo = diffuseColour;
}