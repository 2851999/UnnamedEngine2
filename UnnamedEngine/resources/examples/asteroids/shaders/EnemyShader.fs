#version 140

#map uniform Material_AmbientColour ambientColour
#map uniform Material_DiffuseColour diffuseColour
#map uniform Material_SpecularColour specularColour
#map uniform Light_Direction light_direction
#map uniform Light_DiffuseColour light_diffuseColour
#map uniform Light_SpecularColour light_specularColour
#map uniform CameraPosition camera_position
#map uniform Material_Shininess shininess

uniform vec3 ambientColour;
uniform vec4 diffuseColour;
uniform vec3 specularColour;
uniform vec3 light_direction;
uniform vec3 light_diffuseColour;
uniform vec3 light_specularColour;
uniform vec3 camera_position;
uniform float shininess;

in float frag_visible;
in vec3 frag_position;
in vec3 frag_normal;

out vec4 FragColour;

vec3 calculateDirectionalLight(vec3 diffuseC, vec3 specularColour, vec3 normal) {
	vec3 lightDirection = normalize(-light_direction);
	
	float diffuseStrength = max(dot(lightDirection, normal), 0.0); //When angle > 90 dot product gives negative value
	vec3 diffuseLight = diffuseStrength * (light_diffuseColour * diffuseC);
	
	vec3 viewDirection = normalize(camera_position - frag_position);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	
	float specularStrength = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
	vec3 specularLight = specularStrength * (light_specularColour * specularColour);
	
	return diffuseLight + specularLight;
}

void main() {
	if (frag_visible > 0.5) {
		vec3 normal = normalize(frag_normal);

		FragColour = vec4(vec3(0.1, 0.1, 0.1) + calculateDirectionalLight(vec3(diffuseColour), specularColour, normal), 1.0);
	} else
		discard;
}