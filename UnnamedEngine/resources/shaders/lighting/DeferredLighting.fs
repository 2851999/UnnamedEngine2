#include "Lighting.fs"

layout(set = 4, binding = 0) uniform sampler2D ue_gPosition;
layout(set = 4, binding = 1) uniform sampler2D ue_gNormal;
layout(set = 4, binding = 2) uniform sampler2D ue_gAlbedo;

layout(location = 0) out vec4 ue_FragColour;

void main() {
	vec3 fragPosition = texture(ue_gPosition, ue_frag_textureCoord).rgb;

	vec4 albedo = texture(ue_gAlbedo, ue_frag_textureCoord);
	vec3 diffuseColour = albedo.rgb;
	vec3 ambientColour = diffuseColour;
	vec3 specularColour = diffuseColour;
	
	vec4 normalShin = texture(ue_gNormal, ue_frag_textureCoord);
	vec3 normal = normalShin.rgb;
	float shininess = normalShin.a;

	vec4 fragPosLightSpace[MAX_LIGHTS];

	for (int i = 0; i < ue_numLights; i++) {
		if (ue_lights[i].useShadowMap)
			fragPosLightSpace[i] = ue_lightSpaceMatrix[i] * vec4(fragPosition, 1.0);
	}
	
	vec3 light = ueGetLighting(normal, fragPosition, ambientColour, diffuseColour, specularColour, shininess, fragPosLightSpace);

	ue_FragColour = vec4(light, albedo.a);
}