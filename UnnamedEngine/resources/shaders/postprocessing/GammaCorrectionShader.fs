#version 140

uniform sampler2D tex;

in vec2 frag_textureCoord;

out vec4 FragColour;

vec3 ueGammaCorrect(vec3 colour) {
	const float gamma = 2.2;
	return pow(colour, vec3(1.0 / gamma));
}

vec3 ueReinhardToneMapping(vec3 colour) {
	vec3 mapped = colour / (colour + vec3(1.0));

	return mapped;
}

vec3 ueExposureToneMapping(float exposure, vec3 colour) {
	vec3 mapped = vec3(1.0) - exp(-colour * exposure);

	return mapped;
}

void main() {
	//Get the colour
	vec4 colour = texture(tex, frag_textureCoord);

	FragColour = vec4(ueGammaCorrect(ueReinhardToneMapping(colour.rgb)), colour.a);
}