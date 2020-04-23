#include "../Core.vs"

#ifdef UE_SKINNING
#include "../Skinning.vs"
#endif

#ifdef UE_SKINNING
void main() {
	//Assign the vertex position
	if (ue_useSkinning) {
		gl_Position = ue_modelMatrix * ueGetBoneTransform() * vec4(ue_position, 1.0);
	} else {
		gl_Position = ue_modelMatrix * vec4(ue_position, 1.0);
	}
}
#else
void main() {
	gl_Position = ue_modelMatrix * vec4(ue_position, 1.0);
}
#endif