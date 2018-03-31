#include "Lighting.fs"
#include "CDLODTerrain.fs"

// #map uniform GrassTexture grassTexture
// #map uniform SnowTexture snowTexture
// #map uniform StoneTexture stoneTexture

// uniform sampler2D grassTexture;
// uniform sampler2D snowTexture;
// uniform sampler2D stoneTexture;

#define GRASS_END 0.55
#define STONE_START 0.6
#define STONE_END 0.65
#define SNOW_START 0.7

void main() {
	vec3 mat_diffuseColour = ueGetMaterialDiffuse(ue_frag_textureCoord).rgb;

	//FragColour = vec4(1.0, 1.0, 1.0, 1.0);
	// if (ue_frag_height < GRASS_END) {
	// 	mat_diffuseColour = texture(grassTexture, ue_frag_textureCoord).rgb;
	// } else if (ue_frag_height < STONE_START) {
	// 	vec3 grassColour = texture(grassTexture, ue_frag_textureCoord).rgb;
	// 	vec3 stoneColour = texture(stoneTexture, ue_frag_textureCoord).rgb;
	
	// 	mat_diffuseColour = mix(grassColour, stoneColour, (ue_frag_height - GRASS_END) / (STONE_START - GRASS_END));
	// } else if (ue_frag_height < STONE_END) {
	// 	mat_diffuseColour = texture(stoneTexture, ue_frag_textureCoord).rgb;
	// } else if (ue_frag_height < SNOW_START) {
	// 	vec3 stoneColour = texture(stoneTexture, ue_frag_textureCoord).rgb;
	// 	vec3 snowColour = texture(snowTexture, ue_frag_textureCoord).rgb;
	
	// 	mat_diffuseColour = mix(stoneColour, snowColour, (ue_frag_height - STONE_END) / (SNOW_START - STONE_END));
	// } else {
	// 	mat_diffuseColour = texture(snowTexture, ue_frag_textureCoord).rgb;
	// }

	vec3 normal = normalize(ue_frag_normal);

	// float upness = dot(ue_frag_normal, normalize(vec3(0.0, 1.0, 0.0))); //1 when directly upwards

	// mat_diffuseColour = texture(grassTexture, ue_frag_textureCoord).rgb * upness + texture(stoneTexture, ue_frag_textureCoord).rgb * (1 - upness);
	
	//ue_FragColour = vec4(ue_frag_normal, 1.0);
	
	vec3 ambientColour = mat_diffuseColour;
	vec3 diffuseColour = mat_diffuseColour;
	vec3 specularColour = 0.5 * mat_diffuseColour;
	vec3 light = ueGetLighting(normal, ambientColour, diffuseColour, specularColour);

	ue_FragColour = vec4(light, 1.0);
	
//	ue_FragColour = vec4(ue_frag_height, ue_frag_height, ue_frag_height, 1.0);
}