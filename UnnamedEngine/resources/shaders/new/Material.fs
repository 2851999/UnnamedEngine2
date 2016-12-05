#map uniform Material_AmbientColour material.ambientColour
#map uniform Material_AmbientTexture material.ambientTexture
#map uniform Material_DiffuseColour material.diffuseColour
#map uniform Material_DiffuseTexture material.diffuseTexture
#map uniform Material_SpecularColour material.specularColour
#map uniform Material_SpecularTexture material.specularTexture
#map uniform Material_NormalMap material.normalMap
#map uniform Material_Shininess material.shininess

/* The material structure */
struct ue_Material {
	vec3 ambientColour;
	vec4 diffuseColour;
	vec3 specularColour;
	
	sampler2D ambientTexture;
	sampler2D diffuseTexture;
	sampler2D specularTexture;
	
	sampler2D normalMap;
	
	float shininess;
};

/* The material instance */
uniform ue_Material material;

/* Various methods to get colours */
vec3 ue_getMaterialAmbient(vec2 textureCoord) {
	return material.ambientColour * texture(material.ambientTexture, textureCoord).xyz;
}

vec4 ue_getMaterialDiffuse(vec2 textureCoord) {
	return material.diffuseColour * texture(material.diffuseTexture, textureCoord);
}

vec3 ue_getMaterialSpecular(vec2 textureCoord) {
	return material.specularColour * texture(material.specularTexture, textureCoord).xyz;
}

vec3 ue_getMaterialNormal(vec2 textureCoord) {
	vec3 normal = texture(material.normalMap, textureCoord).rgb;
	normal.y = 1 - normal.y;
	normal = normalize(normal * 2.0 - 1.0);
	return normal;
}

float ue_getMaterialShininess() {
	return material.shininess;
}