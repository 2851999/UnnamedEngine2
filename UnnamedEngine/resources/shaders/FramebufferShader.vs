#include "Core.vs"

void main() {
	ueAssignTextureCoord();
	
	gl_Position = vec4(ue_position.x, ue_position.y, 0.0, 1.0);
}