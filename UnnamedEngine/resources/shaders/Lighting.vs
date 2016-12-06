#include "Core.vs"
#include "Skinning.vs"

uniform mat4 lightSpaceMatrix;
uniform bool useNormalMap;

in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

out vec3 frag_normal;
out vec4 frag_pos_lightspace;
out mat3 frag_tbnMatrix;

void ue_assignLightingData() {
	//Assign the needed data for the fragment shader
	frag_position = vec3(modelMatrix * vec4(position, 1.0));
	frag_normal = normalMatrix * normal;

	mat4 boneTransform;
	if (useSkinning) {
		boneTransform = ue_getBoneTransform();
		
		frag_position = vec3(modelMatrix * boneTransform * vec4(position, 1.0));
		
		frag_normal = normalMatrix * vec3(boneTransform * vec4(normal, 0.0));
	}
	
	frag_pos_lightspace = lightSpaceMatrix * vec4(frag_position, 1.0);
	
	if (useNormalMap) {
		vec3 T;
		vec3 B;
		
		if (useSkinning) {
			T = normalize(normalMatrix * vec3(boneTransform * vec4(tangent, 0.0)));
			B = normalize(normalMatrix * vec3(boneTransform * vec4(bitangent, 0.0)));
		} else {
			T = normalize(normalMatrix * tangent);
			B = normalize(normalMatrix * bitangent);
		}
		vec3 N = normalize(frag_normal);
	
		frag_tbnMatrix = mat3(T, B, N);
	}
	
	//Assign the vertex position
	ue_calculatePosition();
	
	if (useSkinning) {
		gl_Position = mvpMatrix * boneTransform * vec4(position, 1.0);
	}
}