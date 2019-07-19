#version 420

#map uniform Light_Direction light_direction
#map uniform Light_DiffuseColour light_diffuseColour
#map uniform Light_SpecularColour light_specularColour
#map uniform CameraPosition camera_position

layout(binding = 1) uniform sampler2D diffuseTexture;
layout(binding = 4) uniform sampler2D normalMap;

/* The material structure */
struct UEMaterial {
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;
	
	bool hasAmbientTexture;
	bool hasDiffuseTexture;
	bool diffuseTextureSRGB;
	bool hasSpecularTexture;
	bool hasShininessTexture;
	bool hasNormalMap;
	bool hasParallaxMap;
	
	float parallaxScale;
	float shininess;
};

/* The material data */
layout(std140, binding = 2) uniform UEMaterialData {
	UEMaterial ue_material;
};

uniform vec3 light_direction;
uniform vec3 light_diffuseColour;
uniform vec3 light_specularColour;
uniform vec3 camera_position;

in float frag_visible;
in vec3 frag_position;
in vec2 frag_textureCoord;
in vec3 frag_normal;

in mat3 frag_tbnMatrix;

out vec4 FragColour;

vec3 calculateDirectionalLight(vec3 diffuseC, vec3 specularColour, vec3 normal) {
	vec3 lightDirection = normalize(-light_direction);
	
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light_diffuseColour * diffuseC);
	
	vec3 viewDirection = normalize(camera_position - frag_position);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	
	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), ue_material.shininess);
	vec3 specularLight = specularStrength * (light_specularColour * specularColour);
	
	return diffuseLight + specularLight;
}

void main() {
	if (frag_visible > 0.5) {
		vec3 normal = normalize(frag_normal);
		
		if (ue_material.hasNormalMap) {
			normal = texture(normalMap, frag_textureCoord).rgb;
			normal.y = 1 - normal.y;
			
			normal = normalize(normal * 2.0 - 1.0);
			normal = normalize(frag_tbnMatrix * normal);
		}

		FragColour = vec4(vec3(0.1, 0.1, 0.1) + calculateDirectionalLight(vec3(ue_material.diffuseColour * texture(diffuseTexture, frag_textureCoord)), ue_material.specularColour.rgb, normal), 1.0);
	} else
		discard;
}