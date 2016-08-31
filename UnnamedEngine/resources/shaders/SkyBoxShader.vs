#version 140

#map uniform MVPMatrix mvpMatrix
#map attribute Position position

uniform mat4 mvpMatrix;

in vec3 position;

out vec3 frag_textureCoord;

void main() {
	frag_textureCoord = position;
	
	gl_Position = (mvpMatrix * vec4(position, 1.0));
}