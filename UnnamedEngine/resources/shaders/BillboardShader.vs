#include "Core.vs"
#include "Billboard.vs"

#map uniform Billboard_Size billboardSize
#map uniform Billboard_Centre centre

uniform vec2 billboardSize;
uniform vec3 centre;

void main() {
	ue_assignTextureCoord();
	
	//Assign the vertex position
	gl_Position = ue_getBillboardPosition(centre, position, billboardSize);
}