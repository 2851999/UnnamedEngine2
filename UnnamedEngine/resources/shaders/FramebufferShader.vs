#include "Core.vs"

void main() {
	ue_assignTextureCoord();
	
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}