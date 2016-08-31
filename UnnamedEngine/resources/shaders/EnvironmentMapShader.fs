#version 140

#map uniform Material_DiffuseColour material.diffuseColour
#map uniform Material_DiffuseTexture material.diffuseTexture
#map uniform EnvironmentMap environmentMap
#map uniform CameraPosition cameraPos

/* The material structure */
struct Material {
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;
	
	sampler2D ambientTexture;
	sampler2D diffuseTexture;
	sampler2D specularTexture;
};

uniform Material material;
uniform samplerCube environmentMap;
uniform vec3 cameraPos;

in vec2 frag_textureCoord;
in vec3 frag_normal;
in vec3 frag_position;

out vec4 FragColour;

void main() {
	vec3 I = normalize(frag_position - cameraPos);
	vec3 R = reflect(I, normalize(frag_normal));
	
	//vec3 R = refract(I, normalize(frag_normal), 1.00 / 1.52);
	
	//FragColour = material.diffuseColour * texture2D(material.diffuseTexture, frag_textureCoord) * texture(environmentMap, R);
	FragColour = vec4(vec3(material.diffuseColour * texture(material.diffuseTexture, frag_textureCoord) * texture(environmentMap, R)), 1.0);
}