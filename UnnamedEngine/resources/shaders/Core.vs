#include "Core.glsl"

#map uniform MVPMatrix ue_mvpMatrix
#map uniform ModelMatrix ue_modelMatrix
#map uniform NormalMatrix ue_normalMatrix
#map uniform Camera_Position ue_camera_position
#map attribute Position ue_position
#map attribute TextureCoordinate ue_textureCoord
#map attribute Normal ue_normal
#map attribute Tangent ue_tangent
#map attribute Bitangent ue_bitangent

#define UE_LOCATION_POSITION 0
#define UE_LOCATION_TEXTURE_COORD 1
#define UE_LOCATION_NORMAL 2
#define UE_LOCATION_TANGENT 3
#define UE_LOCATION_BITANGENT 4
#define UE_LOCATION_BONE_IDS 5
#define UE_LOCATION_BONE_WEIGHTS 6

uniform mat4 ue_mvpMatrix;
uniform mat4 ue_modelMatrix;
uniform mat3 ue_normalMatrix;

uniform vec3 ue_camera_position;

layout(location = UE_LOCATION_POSITION) in vec3 ue_position;
layout(location = UE_LOCATION_TEXTURE_COORD) in vec2 ue_textureCoord;
layout(location = UE_LOCATION_NORMAL) in vec3 ue_normal;
layout(location = UE_LOCATION_TANGENT) in vec3 ue_tangent;
layout(location = UE_LOCATION_BITANGENT) in vec3 ue_bitangent;

out vec3 ue_frag_position;
out vec2 ue_frag_textureCoord;
out vec3 ue_frag_normal;

void ueAssignPosition() {
	ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
}

void ueAssignTextureCoord() {
	ue_frag_textureCoord = ue_textureCoord;
}

void ueCalculatePosition() {
	gl_Position = ue_mvpMatrix * vec4(ue_position, 1.0);
}