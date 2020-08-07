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

layout(location = 0) in vec3 ue_frag_position;
layout(location = 1) in vec2 ue_frag_textureCoord;
layout(location = 2) in vec3 ue_frag_normal;

//The texture for the particles
layout(set = 1, binding = 1) uniform sampler2D ue_texture;

//Data from the vertex shader
layout(location = 3) in vec4 ue_frag_colour;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	//Assign the colour
	ue_FragColour = ue_frag_colour * texture(ue_texture, ue_frag_textureCoord);
}
