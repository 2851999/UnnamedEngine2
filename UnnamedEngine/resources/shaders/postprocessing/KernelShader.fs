#version 140

uniform sampler2D tex;

in vec2 frag_textureCoord;

out vec4 FragColour;

const float offset = 1.0 / 300; 

void main() {
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),  // top-left
		vec2(0.0f,    offset),  // top-center
		vec2(offset,  offset),  // top-right
		vec2(-offset, 0.0f),    // center-left
		vec2(0.0f,    0.0f),    // center-center
		vec2(offset,  0.0f),    // center-right
		vec2(-offset, -offset), // bottom-left
		vec2(0.0f,    -offset), // bottom-center
		vec2(offset,  -offset)  // bottom-right    
	);
	
	/* SHARPEN
	float kernel[9] = float[](
		-1, -1, -1,
		-1,  9, -1,
		-1, -1, -1
	); */
	
	/* BLUR
	float kernel[9] = float[](
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	); */
	
	/* EDGE DETECTION
	float kernel[9] = float[](
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	); */
	
	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
		sampleTex[i] = vec3(texture(tex, frag_textureCoord.st + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * kernel[i];
	
	FragColour = vec4(col, 1.0);
}