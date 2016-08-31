#version 140

#define MAX_LIGHTS 6

struct Light {
	int  type;

	vec3 position;
	vec3 direction;
	
	vec3 diffuseColour;
	vec3 specularColour;
	
	float constant;
	float linear;
	float quadratic;
	
	float cutoff;
	float outerCutoff;
};

uniform Light lights[MAX_LIGHTS];

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform bool useNormalMap;

uniform mat4 lightSpaceMatrix;

in vec3 position;
in vec2 textureCoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

out vec3 frag_position;
out vec2 frag_textureCoord;
out vec3 frag_normal;
out vec4 frag_pos_lightspace;

out mat3 frag_tbnMatrix;

void main() {
	frag_position = vec3(modelMatrix * vec4(position, 1.0));
	frag_textureCoord = textureCoord;
	frag_normal = normalMatrix * normal;
	
	frag_pos_lightspace = lightSpaceMatrix * vec4(frag_position, 1.0);
	
	gl_Position = mvpMatrix * vec4(position, 1.0);
	
	if (useNormalMap) {
		vec3 T = normalize(normalMatrix * tangent);
		vec3 B = normalize(normalMatrix * bitangent);
		vec3 N = normalize(normalMatrix * normal);
	
		frag_tbnMatrix = mat3(T, B, N);
	}
}