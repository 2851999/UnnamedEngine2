#version 140

#map uniform MVPMatrix mvpMatrix
#map uniform ModelMatrix modelMatrix
#map uniform NormalMatrix normalMatrix
#map attribute Position position
#map attribute TextureCoordinate textureCoord
#map attribute Normal normalMatrix

uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;

in vec3 position;
in vec2 textureCoord;
in vec3 normal;

out vec2 frag_textureCoord;
out vec3 frag_normal;
out vec3 frag_position;

void main() {
	frag_textureCoord = textureCoord;
	
	frag_normal = normalMatrix * normal;
	
	frag_position = vec3(modelMatrix * vec4(position, 1.0f));
	
	gl_Position = mvpMatrix * vec4(position, 1.0);
}