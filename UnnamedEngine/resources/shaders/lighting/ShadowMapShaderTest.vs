#include "../Core.vs"

//Here require ue_modelMatrix = light space matrix * object model matrix (just use same name for convenience)

void main() {
	//mat4 lightSpaceMatrix = transpose(mat4(vec4(-0.1, 0, 0, 0), vec4(0, 0.00001, 0.1, 0), vec4(0, -0.0666667, 0.00000666667, 0), vec4(0, 0, 0, 1)));
	mat4 lightSpaceMatrix = transpose(mat4(vec4(-1, 0, 0, 0), vec4(0, 0.0001, 1, 0), vec4(0, -1.04167, 0.000104167, 4.16667), vec4(0, -1, 0.0001, 5))); //Vulkan perspective
	//mat4 lightSpaceMatrix = transpose(mat4(vec4(-1, 0, 0, 0), vec4(0, 0.0001, 1, 0), vec4(0, -1.08333, 0.000108333, 3.33333), vec4(0, -1, 0.0001, 5))); //OpenGL perspective

	//Assign the vertex position
	gl_Position = lightSpaceMatrix * (ue_modelMatrix * vec4(ue_position, 1.0));
}