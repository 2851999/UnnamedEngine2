#include "Core.vs"
#include "Billboard.vs"

#map attribute PositionsData positionsData
#map attribute TextureData textureData
#map attribute Colour colour

//Mesh data
in vec4 positionsData;  //Contains the position, and size of the current particle
in vec4 textureData;	//Contains the top, left, bottom and right coordinates of the texture for the current particle
in vec4 colour;

out vec4 frag_colour;

void main() {
	//Get the needed data
	vec3 centre = positionsData.xyz;
	float billboardSize = positionsData.w;
	
	float top = textureData.x;
	float left = textureData.y;
	float bottom = textureData.z;
	float right = textureData.w;
	
	//Assign the values needed for the fragment shader
	frag_colour = colour;
	frag_textureCoord = vec2(((position.x + 0.5) * right) - ((position.x - 0.5) * left), ((position.y + 0.5) * top) - ((position.y - 0.5) * bottom));
	
	//Assign the position of the vertex
	gl_Position = ue_getBillboardPosition(centre, position, vec2(billboardSize, billboardSize));
}