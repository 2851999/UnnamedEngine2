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

layout(location = 0) in vec3 ue_frag_position;
layout(location = 1) in vec2 ue_frag_textureCoord;
layout(location = 2) in vec3 ue_frag_normal;


layout(binding = 1) uniform samplerCube ue_tex;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	ue_FragColour = texture(ue_tex, ue_frag_position);
}
