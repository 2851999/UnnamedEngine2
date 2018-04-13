#include "PBRLightingCore.fs"

#map uniform PositionBuffer ue_gPosition
#map uniform NormalBuffer ue_gNormal
#map uniform AlbedoBuffer ue_gAlbedo
#map uniform MetalnessAOBuffer ue_gMetalnessAO

uniform sampler2D ue_gPosition;
uniform sampler2D ue_gNormal;
uniform sampler2D ue_gAlbedo;
uniform sampler2D ue_gMetalnessAO;

uniform mat4 ue_lightSpaceMatrix[MAX_LIGHTS];

in vec2 frag_textureCoord;

out vec4 ue_FragColour;

void main() {
	vec3 fragPosition = texture(ue_gPosition, frag_textureCoord).rgb;

	vec3 albedo = texture(ue_gAlbedo, frag_textureCoord).rgb;
	
	vec4 normalRough = texture(ue_gNormal, frag_textureCoord);
	vec3 normal = normalRough.rgb;
	float roughness = normalRough.a;

	vec3 metalnessAO = texture(ue_gMetalnessAO, frag_textureCoord).rgb;
	float metalness = metalnessAO.r;
	float ao = metalnessAO.g;

	vec4 fragPosLightSpace[MAX_LIGHTS];

	for (int i = 0; i < ue_numLights; i++) {
		if (ue_lights[i].useShadowMap)
			fragPosLightSpace[i] = ue_lightSpaceMatrix[i] * vec4(fragPosition, 1.0);
	}
	
    vec3 colour = ueGetLightingPBR(normal, fragPosition, albedo, metalness, roughness, ao, fragPosLightSpace);

	ue_FragColour = vec4(colour, 1.0);
}