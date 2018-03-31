#include "Lighting.fs"

#map uniform PositionBuffer ue_gPosition
#map uniform NormalBuffer ue_gNormal
#map uniform AlbedoBuffer ue_gAlbedo

uniform sampler2D ue_gPosition;
uniform sampler2D ue_gNormal;
uniform sampler2D ue_gAlbedo;
uniform sampler2D ue_gSpecular;

in vec2 frag_textureCoord;

out vec4 ue_FragColour;

void main() {
	vec3 fragPosition = texture(ue_gPosition, frag_textureCoord).rgb;

	vec4 albedo = texture(ue_gAlbedo, frag_textureCoord);
	vec3 diffuseColour = albedo.rgb;
	vec3 ambientColour = diffuseColour;
	vec3 specularColour = diffuseColour;
	
	vec4 normalShin = texture(ue_gNormal, frag_textureCoord);
	vec3 normal = normalShin.rgb;
	float shininess = normalShin.a;
	
	vec3 light = ueGetLighting(normal, fragPosition, ambientColour, diffuseColour, specularColour, shininess);

	ue_FragColour = vec4(light, albedo.a);
}