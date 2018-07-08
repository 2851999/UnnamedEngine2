#include "Core.vs"

#map attribute Visibility visibility

in float visibility;

out float frag_visibility;

void main() {
	frag_visibility = visibility;

	ueAssignTextureCoord();
	ueCalculatePosition();
}