#include "CDLODTerrain.vs"

void main() {
	ueTerrainAssignTerrainData();

	for (int i = 0; i < ue_numLights; i++)
		ue_frag_pos_lightspace[i] = ue_lightSpaceMatrix[i] * vec4(ue_frag_position, 1.0);
	
	gl_Position = ue_mvpMatrix * vec4(ue_frag_position, 1.0);
}