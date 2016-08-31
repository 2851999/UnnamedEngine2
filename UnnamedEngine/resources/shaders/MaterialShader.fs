#version 140

#map uniform Material_DiffuseColour material.diffuseColour
#map uniform Material_DiffuseTexture material.diffuseTexture

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

in vec2 frag_textureCoord;

out vec4 FragColour;

void main() {
	FragColour = material.diffuseColour * texture2D(material.diffuseTexture, frag_textureCoord);
}