#include "Core.vs"

void main() {
	ue_frag_position = ue_position;

	mat4 rotView = mat4(mat3(ue_viewMatrix));
	vec4 clipPos = ue_projectionMatrix * rotView * vec4(ue_frag_position, 1.0);

	gl_Position = clipPos.xyww;
}