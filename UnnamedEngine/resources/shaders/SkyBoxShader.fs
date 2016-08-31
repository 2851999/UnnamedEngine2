#version 140

#map uniform Material_DiffuseTexture tex

uniform samplerCube tex;

in vec3 frag_textureCoord;

out vec4 FragColor;

void main() {
	FragColor = texture(tex, frag_textureCoord);
}