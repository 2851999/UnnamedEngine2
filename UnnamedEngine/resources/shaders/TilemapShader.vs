#include "Core.vs"

layout(location = 7) in float visibility;

layout(location = 3) out float frag_visibility;

void main() {
	frag_visibility = visibility;

	ueAssignTextureCoord();
	ueCalculatePosition();
}