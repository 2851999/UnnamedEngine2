#version 140

#map attribute Position position
#map attribute TextureCoordinate textureCoord

in vec3 position;
in vec2 textureCoord;

out vec2 frag_textureCoord;

void main() {
	frag_textureCoord = textureCoord;
	
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}