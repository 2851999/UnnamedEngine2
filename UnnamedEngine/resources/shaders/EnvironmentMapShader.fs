#include "Core.fs"
#include "Material.fs"

#map uniform EnvironmentMap ue_environmentMap
#map uniform CameraPosition ue_cameraPos

//The environment map to apply
uniform samplerCube ue_environmentMap;
//The current camera position
uniform vec3 ue_cameraPos;

void main() {
	//Calculate the incident vector
	vec3 I = normalize(ue_frag_position - ue_cameraPos);
	//Now calculate the reflection vector by reflecting about the mesh's normal
	vec3 R = reflect(I, normalize(ue_frag_normal));
	
	//vec3 R = refract(I, normalize(ue_frag_normal), 1.00 / 1.52);
	
	//FragColour = ueGetMaterialDiffuse(ue_frag_textureCoord) * texture(ue_environmentMap, R);
	
	//Assign the colour
	ue_FragColour = vec4(vec3(ueGetMaterialDiffuse(ue_frag_textureCoord) * texture(ue_environmentMap, R)), 1.0);
}