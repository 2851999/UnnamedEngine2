#version 140

#map uniform Material_DiffuseColour colour
#map uniform Material_DiffuseTexture tex

uniform vec4 colour;
uniform sampler2D tex;

in vec2 frag_textureCoord;

out vec4 FragColour;

void main() {
	FragColour = colour * texture2D(tex, frag_textureCoord);
}