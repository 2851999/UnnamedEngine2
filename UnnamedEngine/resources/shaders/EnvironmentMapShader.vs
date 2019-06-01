#include "Core.vs"

void main() {
	//Pass the needed data to the fragment shader
	ueAssignTextureCoord();
	
	ue_frag_normal = mat3(ue_normalMatrix) * ue_normal;
	ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0f));
	
	//Assign the vertex position
	ueCalculatePosition();
}