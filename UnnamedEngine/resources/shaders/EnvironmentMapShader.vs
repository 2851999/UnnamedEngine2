#include "Core.vs"

#map attribute Normal normal

in vec3 normal;

out vec3 frag_normal;

void main() {
	//Pass the needed data to the fragment shader
	ue_assignTextureCoord();
	
	frag_normal = normalMatrix * normal;
	frag_position = vec3(modelMatrix * vec4(position, 1.0f));
	
	//Assign the vertex position
	ue_calculatePosition();
}