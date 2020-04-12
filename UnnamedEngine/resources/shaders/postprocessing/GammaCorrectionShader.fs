#include "PostProcess.fs"

layout (std140, binding = 7) uniform UEGammaCorrectionData {
	vec2 inverseTextureSize;
	bool gammaCorrect;
	float exposureIn;
	bool fxaa;
};

layout(location = 0) out vec4 FragColour;

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

const uniform float fxaaSpanMax = 8.0;
const uniform float fxaaReduceMin = 1.0 / 128.0;
const uniform float fxaaReduceMul = 1.0 / 8.0;

void main() {
	vec4 colour;
	
	if (fxaa) {
		vec3 luma = vec3(0.299, 0.587, 0.114);	
		float lumaTL = dot(luma, texture(ue_texture, ue_frag_textureCoord.xy + (vec2(-1.0, -1.0) * inverseTextureSize)).xyz);
		float lumaTR = dot(luma, texture(ue_texture, ue_frag_textureCoord.xy + (vec2(1.0, -1.0) * inverseTextureSize)).xyz);
		float lumaBL = dot(luma, texture(ue_texture, ue_frag_textureCoord.xy + (vec2(-1.0, 1.0) * inverseTextureSize)).xyz);
		float lumaBR = dot(luma, texture(ue_texture, ue_frag_textureCoord.xy + (vec2(1.0, 1.0) * inverseTextureSize)).xyz);
		float lumaM  = dot(luma, texture(ue_texture, ue_frag_textureCoord.xy).xyz);

		vec2 dir;
		dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
		dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
		
		float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (fxaaReduceMul * 0.25), fxaaReduceMin);
		float inverseDirAdjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
		
		dir = min(vec2(fxaaSpanMax, fxaaSpanMax), max(vec2(-fxaaSpanMax, -fxaaSpanMax), dir * inverseDirAdjustment)) * inverseTextureSize;

		vec3 result1 = (texture(ue_texture, ue_frag_textureCoord.xy + (dir * vec2(1.0 / 3.0 - 0.5))).xyz + texture(ue_texture, ue_frag_textureCoord.xy + (dir * vec2(2.0 / 3.0 - 0.5))).xyz) * 0.5;
		vec3 result2 = result1 * 0.5 + 0.25 * (texture(ue_texture, ue_frag_textureCoord.xy + (dir * vec2(0.0 / 3.0 - 0.5))).xyz + texture(ue_texture, ue_frag_textureCoord.xy + (dir * vec2(3.0 / 3.0 - 0.5))).xyz);

		float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
		float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
		float lumaResult2 = dot(luma, result2);
		
		if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
			colour = vec4(result1, 1.0);
		else
			colour = vec4(result2, 1.0);
	} else
		//Obtain the colour from the rendered image
		colour = texture(ue_texture, ue_frag_textureCoord);

	//Apply gamma correction/exposure mapping
	vec3 colourOutput;

	if (exposureIn >= 0)
		colourOutput = ueExposureToneMapping(exposureIn, colour.rgb);
	else
		colourOutput = ueReinhardToneMapping(colour.rgb); //Default to reinhard

	if (gammaCorrect)
		colourOutput = ueGammaCorrect(colourOutput);

	FragColour = vec4(colourOutput, colour.a);
}