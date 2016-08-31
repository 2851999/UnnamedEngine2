#version 140

#map uniform MVPMatrix mvpMatrix
#map attribute Position position
#map attribute TextureCoordinate textureCoord

uniform mat4 mvpMatrix;

in vec3 position;
in vec2 textureCoord;

out vec2 frag_textureCoord;

void main() {
	frag_textureCoord = textureCoord;
	
	gl_Position = mvpMatrix * vec4(position, 1.0);
}