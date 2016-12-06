#include "Core.fs"

#map uniform Material_DiffuseColour colour
#map uniform Material_DiffuseTexture tex

uniform sampler2D tex;
uniform vec4 colour;

void main() {
	FragColour = vec4(1.0, 1.0, 1.0, texture(tex, frag_textureCoord).r) * colour;
}