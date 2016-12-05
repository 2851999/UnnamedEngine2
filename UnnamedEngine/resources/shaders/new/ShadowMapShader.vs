#include "Core.vs"
#include "Skinning.vs"

#map uniform LightSpaceMatrix mat

uniform mat4 mat;

void main() {
	//Assign the vertex position
	if (useSkinning) {
		gl_Position = mat * ue_getBoneTransform() * vec4(position, 1.0);
	} else {
		gl_Position = mat * vec4(position, 1.0);
	}
}