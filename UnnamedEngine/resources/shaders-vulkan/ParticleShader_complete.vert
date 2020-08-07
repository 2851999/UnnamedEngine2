#version 420

//Used for assigning UBO block locations - can remove

/* NOTES:
* Set 0 - Varies per camera
* Set 1 - Varies per material
* Set 2 - Varies per model
* Set 3 - Varies per light batch
*/

layout(std140, set = 0, binding = 21) uniform UECameraData {
	mat4 ue_viewMatrix;
	mat4 ue_projectionMatrix;
	
	vec4 ue_cameraPosition;
};

layout(std140, set = 2, binding = 22) uniform UEModelData {
	mat4 ue_mvpMatrix;
	mat4 ue_modelMatrix;
	mat4 ue_normalMatrix;
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

layout(std140, set = 3, binding = 31) uniform UEBillboardData {
	mat4 ue_projectionViewMatrix;
	vec4 ue_cameraRight;
	vec4 ue_cameraUp;
	vec4 ue_billboardCentre;
	vec2 ue_billboardSize;
};

vec4 ueGetBillboardPosition(vec3 cent, vec3 pos, vec2 size) {
	return ue_projectionViewMatrix * vec4(cent + ((ue_cameraRight.xyz * pos.x * size.x) + (ue_cameraUp.xyz * pos.y * size.y)), 1.0);
}

//Mesh data
layout(location = 7) in vec4 ue_positionsData;  //Contains the position, and size of the current particle
layout(location = 8) in vec4 ue_colour;
layout(location = 9) in vec4 ue_textureData;	//Contains the top, left, bottom and right coordinates of the texture for the current particle

layout(location = 3) out vec4 ue_frag_colour;

void main() {
	//Get the needed data
	vec3 centre = ue_positionsData.xyz;
	float billboardSize = ue_positionsData.w;
	
	float top = ue_textureData.x;
	float left = ue_textureData.y;
	float bottom = ue_textureData.z;
	float right = ue_textureData.w;
	
	//Assign the values needed for the fragment shader
	ue_frag_colour = ue_colour;
	ue_frag_textureCoord = vec2(((ue_position.x + 0.5) * right) - ((ue_position.x - 0.5) * left), ((ue_position.y + 0.5) * top) - ((ue_position.y - 0.5) * bottom));
	
	//Assign the position of the vertex
	gl_Position = ueGetBillboardPosition(centre, ue_position, vec2(billboardSize, billboardSize));
}
