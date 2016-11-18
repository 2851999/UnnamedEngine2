#version 140

#map uniform Material_DiffuseColour diffuseColour

uniform vec4 diffuseColour;

in float frag_visible;
in vec3 frag_position;

out vec4 FragColour;

void main() {
	if (frag_visible > 0.5) {
		FragColour = diffuseColour;
	} else
		discard;
}