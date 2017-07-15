#version 330

/*#map uniform GrassTexture grassTexture
#map uniform SnowTexture snowTexture
#map uniform StoneTexture stoneTexture

uniform sampler2D grassTexture;
uniform sampler2D snowTexture;
uniform sampler2D stoneTexture; */

//in vec2 frag_textureCoord;
in float frag_height;

out vec4 FragColour;

/*#define GRASS_END 0.5
#define STONE_START 0.70
#define STONE_END 0.70
#define SNOW_START 0.8*/

void main() {
	//FragColour = vec4(1.0, 1.0, 1.0, 1.0);
/*	if (frag_height < GRASS_END) {
		FragColour = vec4(texture(grassTexture, frag_textureCoord).rgb, 1.0);
	} else if (frag_height < STONE_START) {
		vec3 grassColour = texture(grassTexture, frag_textureCoord).rgb;
		vec3 stoneColour = texture(stoneTexture, frag_textureCoord).rgb;
	
		FragColour = vec4(mix(grassColour, stoneColour, (frag_height - GRASS_END) / (STONE_START - GRASS_END)), 1.0);
	} else if (frag_height < STONE_END) {
		FragColour = vec4(texture(stoneTexture, frag_textureCoord).rgb, 1.0);
	} else if (frag_height < SNOW_START) {
		vec3 stoneColour = texture(stoneTexture, frag_textureCoord).rgb;
		vec3 snowColour = texture(snowTexture, frag_textureCoord).rgb;
	
		FragColour = vec4(mix(stoneColour, snowColour, (frag_height - STONE_END) / (SNOW_START - STONE_END)), 1.0);
	} else {
		FragColour = vec4(texture(snowTexture, frag_textureCoord).rgb, 1.0);
	} */
	
	
	FragColour = vec4(frag_height, frag_height, frag_height, 1.0);
/*	vec3 colour = vec3(frag_colour, 1.0, frag_colour);
	if (frag_colour < 0.1) {
		colour = vec3(frag_colour, frag_colour, 1.0);
	}
	FragColour = vec4(colour, 1.0); */
}