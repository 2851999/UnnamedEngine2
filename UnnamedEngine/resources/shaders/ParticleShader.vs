#include "Core.vs"
#include "billboard/Billboard.vs"

#map attribute PositionsData ue_positionsData
#map attribute TextureData ue_textureData
#map attribute Colour ue_colour

//Mesh data
in vec4 ue_positionsData;  //Contains the position, and size of the current particle
in vec4 ue_textureData;	//Contains the top, left, bottom and right coordinates of the texture for the current particle
in vec4 ue_colour;

out vec4 ue_frag_colour;

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