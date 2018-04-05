#include "../Core.vs"
#include "../Skinning.vs"

#map uniform LightSpaceMatrix ue_mat

uniform mat4 ue_mat;

void main() {
	//Assign the vertex position
	if (ue_useSkinning) {
		gl_Position = ue_mat * ueGetBoneTransform() * vec4(ue_position, 1.0);
	} else {
		gl_Position = ue_mat * vec4(ue_position, 1.0);
	}
}