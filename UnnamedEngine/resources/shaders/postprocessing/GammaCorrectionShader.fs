#include "PostProcess.fs"

#map uniform GammaCorrect gammaCorrect
#map uniform Exposure exposureIn

uniform bool gammaCorrect;
uniform float exposureIn;

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
	vec4 colour = texture(ue_texture, ue_frag_textureCoord);
	vec3 colourOutput;

	if (exposureIn >= 0)
		colourOutput = ueExposureToneMapping(exposureIn, colour.rgb);
	else
		colourOutput = ueReinhardToneMapping(colour.rgb); //Default to reinhard

	if (gammaCorrect)
		colourOutput = ueGammaCorrect(colourOutput);

	FragColour = vec4(colourOutput, colour.a);
}