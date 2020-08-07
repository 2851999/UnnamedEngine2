#include "../Core.vs"
#include "../Material.glsl"

#ifdef UE_SKINNING
#include "../Skinning.vs"
#endif

#ifndef UE_GEOMETRY_ONLY
#include "Lighting.glsl"
#endif

layout(location = 7) out vec3 ue_tangentViewPos;
layout(location = 8) out vec3 ue_tangentFragPos;

layout(location = 9) out mat3 ue_frag_tbnMatrix;
#ifndef UE_GEOMETRY_ONLY
layout(location = 13) out vec4 ue_frag_pos_lightspace[MAX_LIGHTS];
#endif

#ifdef UE_SKINNING
void ueAssignLightingData() {
	mat4 boneTransform;
	mat3 normalMatrix = mat3(ue_normalMatrix);
	if (ue_useSkinning) {
		boneTransform = ueGetBoneTransform();
		
		ue_frag_position = vec3(ue_modelMatrix * boneTransform * vec4(ue_position, 1.0));
		
		ue_frag_normal = normalMatrix * vec3(boneTransform * vec4(ue_normal, 0.0));
	} else {
		ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
		ue_frag_normal = normalMatrix * ue_normal;
	}
#ifndef UE_GEOMETRY_ONLY
	for (int i = 0; i < ue_numLights; i++)
		ue_frag_pos_lightspace[i] = ue_lightSpaceMatrix[i] * vec4(ue_frag_position, 1.0);
#endif
	
	if (ue_material.hasNormalMap) {
		vec3 T;
		vec3 B;
		
		if (ue_useSkinning) {
			T = normalize(normalMatrix * vec3(boneTransform * vec4(ue_tangent, 0.0)));
			B = normalize(normalMatrix * vec3(boneTransform * vec4(ue_bitangent, 0.0)));
		} else {
			T = normalize(normalMatrix * ue_tangent);
			B = normalize(normalMatrix * ue_bitangent);
		}
		vec3 N = normalize(ue_frag_normal);
	
		ue_frag_tbnMatrix = mat3(-T, B, N);
		
		if (ue_material.hasParallaxMap) {
			ue_tangentViewPos = transpose(ue_frag_tbnMatrix) * ue_cameraPosition.xyz;
			ue_tangentFragPos = transpose(ue_frag_tbnMatrix) * ue_frag_position;
		}
	}
	
	//Assign the vertex position
	if (ue_useSkinning) {
		gl_Position = ue_mvpMatrix * boneTransform * vec4(ue_position, 1.0);
	} else {
		ueCalculatePosition();
	}
}
#else
void ueAssignLightingData() {
	mat3 normalMatrix = mat3(ue_normalMatrix);
	ue_frag_position = vec3(ue_modelMatrix * vec4(ue_position, 1.0));
	ue_frag_normal = normalMatrix * ue_normal;
	
#ifndef UE_GEOMETRY_ONLY
	for (int i = 0; i < ue_numLights; i++)
		ue_frag_pos_lightspace[i] = ue_lightSpaceMatrix[i] * vec4(ue_frag_position, 1.0);
#endif
	
	if (ue_material.hasNormalMap) {
		vec3 T = normalize(normalMatrix * ue_tangent);
		vec3 B = normalize(normalMatrix * ue_bitangent);
		vec3 N = normalize(ue_frag_normal);
	
		ue_frag_tbnMatrix = mat3(-T, B, N);
		
		if (ue_material.hasParallaxMap) {
			ue_tangentViewPos = transpose(ue_frag_tbnMatrix) * ue_cameraPosition.xyz;
			ue_tangentFragPos = transpose(ue_frag_tbnMatrix) * ue_frag_position;
		}
	}
	
	//Assign the vertex position
	ueCalculatePosition();
}
#endif