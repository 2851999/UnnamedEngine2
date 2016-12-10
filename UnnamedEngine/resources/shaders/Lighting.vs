#include "Core.vs"
#include "Skinning.vs"

uniform mat4 ue_lightSpaceMatrix;
uniform bool ue_useNormalMap;
uniform bool ue_useParallaxMap;

out vec4 ue_frag_pos_lightspace;
out mat3 ue_frag_tbnMatrix;

out vec3 ue_tangentViewPos;
out vec3 ue_tangentFragPos;

void ueAssignLightingData() {
	mat4 boneTransform;
	if (ue_useSkinning) {
		boneTransform = ueGetBoneTransform();
		
		ue_frag_position = vec3(ue_modelMatrix * boneTransform * vec4(ue_position, 1.0));
		
		ue_frag_normal = ue_normalMatrix * vec3(boneTransform * vec4(ue_normal, 0.0));
	} else {
		ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
		ue_frag_normal = ue_normalMatrix * ue_normal;
	}
	
	ue_frag_pos_lightspace = ue_lightSpaceMatrix * vec4(ue_frag_position, 1.0);
	
	if (ue_useNormalMap) {
		vec3 T;
		vec3 B;
		
		if (ue_useSkinning) {
			T = normalize(ue_normalMatrix * vec3(boneTransform * vec4(ue_tangent, 0.0)));
			B = normalize(ue_normalMatrix * vec3(boneTransform * vec4(ue_bitangent, 0.0)));
		} else {
			T = normalize(ue_normalMatrix * ue_tangent);
			B = normalize(ue_normalMatrix * ue_bitangent);
		}
		vec3 N = normalize(ue_frag_normal);
	
		ue_frag_tbnMatrix = mat3(T, B, N);
		
		if (ue_useParallaxMap) {
			ue_tangentViewPos = ue_frag_tbnMatrix * ue_camera_position;
			ue_tangentFragPos = ue_frag_tbnMatrix * ue_frag_position;
		}
	}
	
	//Assign the vertex position
	if (ue_useSkinning) {
		gl_Position = ue_mvpMatrix * boneTransform * vec4(ue_position, 1.0);
	} else {
		ueCalculatePosition();
	}
}