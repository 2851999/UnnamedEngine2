#version 420
//Used for assigning UBO block locations - can remove

layout (std140, binding = 1) uniform UECoreData {
	uniform mat4 ue_mvpMatrix;
	uniform mat4 ue_modelMatrix;
	uniform mat4 ue_viewMatrix;
	uniform mat4 ue_projectionMatrix;
	uniform mat4 ue_normalMatrix;
	
	uniform vec4 ue_cameraPosition;
};