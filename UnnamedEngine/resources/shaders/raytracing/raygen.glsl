
//Camera stuff
layout(std140, set = 0, binding = 1) uniform UECameraData {
	mat4 ue_viewMatrix;
	mat4 ue_projectionMatrix;
	
	vec4 ue_cameraPosition;
};

//TLAS
layout(set = 1, binding = 0) uniform accelerationStructureEXT tlas;

//Storage image
layout(set = 1, binding = 1, rgba8) uniform image2D image;

//Push constants
layout(push_constant) uniform PushConstants {
	int frame;
} pushConstants;