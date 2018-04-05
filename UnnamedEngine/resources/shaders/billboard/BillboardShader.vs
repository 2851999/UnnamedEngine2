#include "../Core.vs"
#include "Billboard.vs"

#map uniform Billboard_Size ue_billboardSize
#map uniform Billboard_Centre ue_centre

uniform vec2 ue_billboardSize;
uniform vec3 ue_centre;

void main() {
	ueAssignTextureCoord();
	
	//Assign the vertex position
	gl_Position = ueGetBillboardPosition(ue_centre, ue_position, ue_billboardSize);
}