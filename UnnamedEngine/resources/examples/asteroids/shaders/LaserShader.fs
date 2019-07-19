#version 420

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

in float frag_visible;
in vec3 frag_position;

out vec4 FragColour;

void main() {
	if (frag_visible > 0.5) {
		FragColour = ue_material.diffuseColour;
	} else
		discard;
}