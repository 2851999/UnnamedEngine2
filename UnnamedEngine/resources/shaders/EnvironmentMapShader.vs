#version 140

#map uniform MVPMatrix mvpMatrix
#map uniform ModelMatrix modelMatrix
#map uniform NormalMatrix normalMatrix
#map attribute Position position
#map attribute TextureCoordinate textureCoord
#map attribute Normal normalMatrix

//Matrices
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelMatrix;

//Mesh data
in vec3 position;
in vec2 textureCoord;
in vec3 normal;

//Data for the fragment shader
out vec2 frag_textureCoord;
out vec3 frag_normal;
out vec3 frag_position;

void main() {
	//Pass the needed data to the fragment shader
	frag_textureCoord = textureCoord;
	frag_normal = normalMatrix * normal;
	frag_position = vec3(modelMatrix * vec4(position, 1.0f));
	
	//Assign the vertex position
	gl_Position = mvpMatrix * vec4(position, 1.0);
}