#version 330

//#include "Lighting.fs"

#map uniform GrassTexture grassTexture
#map uniform SnowTexture snowTexture
#map uniform StoneTexture stoneTexture

uniform sampler2D grassTexture;
uniform sampler2D snowTexture;
uniform sampler2D stoneTexture;

in vec2 ue_frag_textureCoord;

uniform vec3 cameraPosition;

in float frag_height;
in vec3 ue_frag_normal;
in vec3 frag_worldPosition;

out vec4 ue_FragColour;

#define GRASS_END 0.5
#define STONE_START 0.70
#define STONE_END 0.70
#define SNOW_START 0.8

vec3 ueCalculateDirectionalLight(vec3 mat_diffuseColour) {
	vec3 light_direction = vec3(0.0, -1.0, 0.0001);
	vec3 light_diffuseColour = vec3(1.0, 1.0, 1.0);
	vec3 light_specularColour = vec3(1.0, 1.0, 1.0);

	vec3 mat_specularColour = 0.5 * mat_diffuseColour;
	float mat_shininess = 1;

	vec3 normal = normalize(ue_frag_normal);


	vec3 lightDirection = normalize(-light_direction);
	
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light_diffuseColour * mat_diffuseColour);

	vec3 viewDirection = normalize(cameraPosition - frag_worldPosition);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	vec3 specularLight = vec3(0.0);

	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), mat_shininess);
	specularLight = specularStrength * (light_specularColour * mat_specularColour);

	vec3 ambientLight = vec3(0.1, 0.1, 0.1) * mat_diffuseColour;

	return ambientLight + diffuseLight + specularLight;
}

void main() {
	vec3 mat_diffuseColour = vec3(1.0, 1.0, 1.0);

	//FragColour = vec4(1.0, 1.0, 1.0, 1.0);
	if (frag_height < GRASS_END) {
		mat_diffuseColour = texture(grassTexture, ue_frag_textureCoord).rgb;
	} else if (frag_height < STONE_START) {
		vec3 grassColour = texture(grassTexture, ue_frag_textureCoord).rgb;
		vec3 stoneColour = texture(stoneTexture, ue_frag_textureCoord).rgb;
	
		mat_diffuseColour = mix(grassColour, stoneColour, (frag_height - GRASS_END) / (STONE_START - GRASS_END));
	} else if (frag_height < STONE_END) {
		mat_diffuseColour = texture(stoneTexture, ue_frag_textureCoord).rgb;
	} else if (frag_height < SNOW_START) {
		vec3 stoneColour = texture(stoneTexture, ue_frag_textureCoord).rgb;
		vec3 snowColour = texture(snowTexture, ue_frag_textureCoord).rgb;
	
		mat_diffuseColour = mix(stoneColour, snowColour, (frag_height - STONE_END) / (SNOW_START - STONE_END));
	} else {
		mat_diffuseColour = texture(snowTexture, ue_frag_textureCoord).rgb;
	}
	
	//ue_FragColour = vec4(ue_frag_normal, 1.0);
	//ue_FragColour = vec4(ueGetLighting(normalize(ue_frag_normal), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f)), 1.0);
	ue_FragColour = vec4(ueCalculateDirectionalLight(mat_diffuseColour), 1.0);
	
	//FragColour = vec4(frag_height, frag_height, frag_height, 1.0);
/*	vec3 colour = vec3(frag_colour, 1.0, frag_colour);
	if (frag_colour < 0.1) {
		colour = vec3(frag_colour, frag_colour, 1.0);
	}
	FragColour = vec4(colour, 1.0); */
}