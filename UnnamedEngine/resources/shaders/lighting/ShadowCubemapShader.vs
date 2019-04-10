#include "../Core.vs"
#include "../Skinning.vs"

void main() {
	//Assign the vertex position
	if (ue_useSkinning) {
		gl_Position = ue_modelMatrix * ueGetBoneTransform() * vec4(ue_position, 1.0);
	} else {
		gl_Position = ue_modelMatrix * vec4(ue_position, 1.0);
	}
}