#include "Core.fs"
#include "Material.fs"

#map uniform EnvironmentMap environmentMap
#map uniform CameraPosition cameraPos

//The environment map to apply
uniform samplerCube environmentMap;
//The current camera position
uniform vec3 cameraPos;

//Data from the vertex shader
in vec3 frag_normal;

void main() {
	//Calculate the incident vector
	vec3 I = normalize(frag_position - cameraPos);
	//Now calculate the reflection vector by reflecting about the mesh's normal
	vec3 R = reflect(I, normalize(frag_normal));
	
	//vec3 R = refract(I, normalize(frag_normal), 1.00 / 1.52);
	
	//FragColour = ue_getMaterialDiffuse(frag_textureCoord) * texture(environmentMap, R);
	
	//Assign the colour
	FragColour = vec4(vec3(ue_getMaterialDiffuse(frag_textureCoord) * texture(environmentMap, R)), 1.0);
}