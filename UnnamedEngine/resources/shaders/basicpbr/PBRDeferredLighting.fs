#include "PBRLightingCore.fs"

layout(set = 4, binding = 0) uniform sampler2D ue_gPosition;
layout(set = 4, binding = 1) uniform sampler2D ue_gNormal;
layout(set = 4, binding = 2) uniform sampler2D ue_gAlbedo;
layout(set = 4, binding = 3) uniform sampler2D ue_gMetalnessAO;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	vec3 fragPosition = texture(ue_gPosition, ue_frag_textureCoord).rgb;
	vec4 albedo = texture(ue_gAlbedo, ue_frag_textureCoord);
	
	vec4 normalRough = texture(ue_gNormal, ue_frag_textureCoord);
	vec3 normal = normalRough.rgb;
	float roughness = normalRough.a;

	vec3 metalnessAO = texture(ue_gMetalnessAO, ue_frag_textureCoord).rgb;
	float metalness = metalnessAO.r;
	float ao = metalnessAO.g;

	vec4 fragPosLightSpace[MAX_LIGHTS];

	for (int i = 0; i < ue_numLights; i++) {
		if (ue_lights[i].useShadowMap && ue_lights[i].type != 2)
			fragPosLightSpace[i] = ue_lightSpaceMatrix[i] * vec4(fragPosition, 1.0);
	}
	
    vec3 colour = ueGetLightingPBR(normal, fragPosition, albedo.rgb, metalness, roughness, ao, fragPosLightSpace);
	
	ue_FragColour = vec4(colour, albedo.a);
}