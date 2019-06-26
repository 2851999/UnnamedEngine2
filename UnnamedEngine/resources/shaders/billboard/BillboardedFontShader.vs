#include "../Core.vs"
#include "Billboard.vs"

void main() {
	ueAssignTextureCoord();
	
	gl_Position = ueGetBillboardPosition(ue_billboardCentre.xyz, ue_position, ue_billboardSize);
}