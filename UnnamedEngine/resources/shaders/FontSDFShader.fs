#include "Core.fs"
#include "Material.glsl"

layout(location = 0) out vec4 ue_FragColour;

/* Various text rendering parameters */
layout(std140, set = 3, binding = 4) uniform UESDFTextData {
	vec4 outlineColour;
	vec4 shadowColour;
	float smoothing;
	float outline;
	float shadow;
	float shadowSmoothing;
	vec2 shadowOffset;
};

//SEE: https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf

/*
void main() {
	//Input values
	float softEdges = 1.0;

	float SOFT_EDGE_MIN = 0.4;
	float SOFT_EDGE_MAX = 0.5;

	vec4 baseColour = ue_material.diffuseColour;
	float distAlphaMask = texture(ue_material_diffuseTexture, ue_frag_textureCoord).a;

	if (softEdges > 0.0)
		baseColour.a *= smoothstep(SOFT_EDGE_MIN, SOFT_EDGE_MAX, distAlphaMask);
	else
		baseColour.a = (distAlphaMask >= 0.5 ? 1.0 : 0.0);

	ue_FragColour = baseColour;
}
*/
/* https://github.com/libgdx/libgdx/wiki/Distance-field-fonts
void main() {
	//Input values
	float smoothing = 1.0 / 16.0; //Between 0 and 0.5
	vec4 outlineColour = vec4(0.0, 1.0, 0.0, 1.0);
	float outline = 0.1; //1.0 thick, 0 none
	float shadow = 1.0;
	vec2 shadowOffset = vec2(0.01, 0.01);
	float shadowSmoothing = 0.1; //Between 0 and 0.5
	vec4 shadowColour = vec4(0.0, 0.0, 0.0, 1.0);

	float outlineDistance = outline > 0.0 ? (0.5 - outline*0.5) : 0.5; //0.5 none, 0.0 thick

	vec4 baseColour = ue_material.diffuseColour;
	float distAlphaMask = texture(ue_material_diffuseTexture, ue_frag_textureCoord).a;

	if (outline > 0.0) {
		float outlineFactor = smoothstep(0.5 - smoothing, 0.5 + smoothing, distAlphaMask);
		baseColour = mix(outlineColour, baseColour, outlineFactor);

		if (smoothing > 0.0)
			baseColour.a *= smoothstep(outlineDistance - smoothing, outlineDistance + smoothing, distAlphaMask);
		else
			baseColour.a = (distAlphaMask >= outlineDistance ? 1.0 : 0.0);
	} else {
		if (smoothing > 0.0)
			baseColour.a *= smoothstep(0.5 - smoothing, 0.5 + smoothing, distAlphaMask);
		else
			baseColour.a = (distAlphaMask >= 0.5 ? 1.0 : 0.0);
	}

	if (shadow > 0.0) {
		float shadowDistance = texture(ue_material_diffuseTexture, ue_frag_textureCoord - shadowOffset).a;
		float shadowAlpha = smoothstep(0.5 - shadowSmoothing, 0.5 + shadowSmoothing, shadowDistance);
		vec4 shadowValue = vec4(shadowColour.rgb, shadowColour.a * shadowAlpha);

		ue_FragColour = mix(shadowValue, baseColour, baseColour.a);
	} else {
		ue_FragColour = baseColour;
	}
}
*/

void main() {
	//Input values
	float smoothingValue = smoothing * 0.5; //Need between 0 and 0.5
	float shadowSmoothingValue = shadowSmoothing * 0.5; //Need between 0 and 0.5

	vec4 baseColour = ue_material.diffuseColour;
	float distAlphaMask = texture(ue_material_diffuseTexture, ue_frag_textureCoord).a;

	if (outline > 0.0) {
		float outlineDistance = outline > 0.0 ? (0.5 - outline*0.5) : 0.5; //0.5 none, 0.0 thick
		float outlineFactor = smoothstep(0.5 - smoothingValue, 0.5 + smoothingValue, distAlphaMask);
		baseColour = mix(outlineColour, baseColour, outlineFactor);

		if (smoothingValue > 0.0)
			baseColour.a *= smoothstep(outlineDistance - smoothingValue, outlineDistance + smoothingValue, distAlphaMask);
		else
			baseColour.a = (distAlphaMask >= outlineDistance ? 1.0 : 0.0);
	} else {
		if (smoothingValue > 0.0)
			baseColour.a *= smoothstep(0.5 - smoothingValue, 0.5 + smoothingValue, distAlphaMask);
		else
			baseColour.a = (distAlphaMask >= 0.5 ? 1.0 : 0.0);
	}

	if (shadow > 0.0) {
		float shadowDistance = texture(ue_material_diffuseTexture, ue_frag_textureCoord - shadowOffset).a;
		float shadowAlpha = smoothstep(0.5 - shadowSmoothingValue, 0.5 + shadowSmoothingValue, shadowDistance);
		vec4 shadowValue = vec4(shadowColour.rgb, shadowColour.a * shadowAlpha);

		ue_FragColour = mix(shadowValue, baseColour, baseColour.a);
	} else {
		ue_FragColour = baseColour;
	}
}