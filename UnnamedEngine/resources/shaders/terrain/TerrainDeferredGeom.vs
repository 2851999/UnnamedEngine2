#include "CDLODTerrain.vs"

void main() {
	ueTerrainAssignTerrainData();
	
	gl_Position = ue_mvpMatrix * vec4(ue_frag_position, 1.0);
}