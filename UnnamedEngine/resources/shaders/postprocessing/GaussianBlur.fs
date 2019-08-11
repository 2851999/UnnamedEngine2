#include "PostProcess.fs"

#map uniform Horizontal horizontal
#map uniform Texture1 ue_brightTexture

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform sampler2D ue_brightTexture;

layout(location = 0) out vec4 ue_FragColour;
layout(location = 1) out vec4 ue_FragColourBright;

void main() {             
	vec2 tex_offset = 1.0 / textureSize(ue_brightTexture, 0); // gets size of single texel
	vec3 result = texture(ue_brightTexture, ue_frag_textureCoord).rgb * weight[0]; // current fragment's contribution
	if(horizontal) {
		for(int i = 1; i < 5; ++i) {
			result += texture(ue_brightTexture, ue_frag_textureCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			result += texture(ue_brightTexture, ue_frag_textureCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	} else {
		for(int i = 1; i < 5; ++i) {
			result += texture(ue_brightTexture, ue_frag_textureCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(ue_brightTexture, ue_frag_textureCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	ue_FragColour = vec4(texture(ue_texture, ue_frag_textureCoord).rgb, 1.0);
	ue_FragColourBright = vec4(result, 1.0);
}