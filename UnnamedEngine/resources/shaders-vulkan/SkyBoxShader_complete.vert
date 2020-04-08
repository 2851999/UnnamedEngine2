#version 420
//Used for assigning UBO block locations - can remove

layout(std140, binding = 21) uniform UECoreData {
	mat4 ue_mvpMatrix;
	mat4 ue_modelMatrix;
	mat4 ue_viewMatrix;
	mat4 ue_projectionMatrix;
	mat4 ue_normalMatrix;
	
	vec4 ue_cameraPosition;
};


#define UE_LOCATION_POSITION 0
#define UE_LOCATION_TEXTURE_COORD 1
#define UE_LOCATION_NORMAL 2
#define UE_LOCATION_TANGENT 3
#define UE_LOCATION_BITANGENT 4
#define UE_LOCATION_BONE_IDS 5
#define UE_LOCATION_BONE_WEIGHTS 6

layout(location = UE_LOCATION_POSITION) in vec3 ue_position;
layout(location = UE_LOCATION_TEXTURE_COORD) in vec2 ue_textureCoord;
layout(location = UE_LOCATION_NORMAL) in vec3 ue_normal;
layout(location = UE_LOCATION_TANGENT) in vec3 ue_tangent;
layout(location = UE_LOCATION_BITANGENT) in vec3 ue_bitangent;

layout(location = 0) out vec3 ue_frag_position;
layout(location = 1) out vec2 ue_frag_textureCoord;
layout(location = 2) out vec3 ue_frag_normal;

void ueAssignPosition() {
	//Might want to change like CDLOD terrain
	ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
}

void ueAssignTextureCoord() {
	ue_frag_textureCoord = ue_textureCoord;
}

void ueCalculatePosition() {
	gl_Position = ue_mvpMatrix * vec4(ue_position, 1.0);
}

void main() {
	ue_frag_position = ue_position;

	mat4 rotView = mat4(mat3(ue_viewMatrix));
	vec4 clipPos = ue_projectionMatrix * rotView * vec4(ue_frag_position, 1.0);

	gl_Position = clipPos.xyww;
}
