#version 140

#map uniform MVPMatrix mvpMatrix
#map uniform ProjectionViewMatrix projectionViewMatrix
#map uniform Camera_Right cameraRight
#map uniform Camera_Up cameraUp
#map uniform Billboard_Size billboardSize
#map uniform Billboard_Centre centre
#map attribute Position position
#map attribute TextureCoordinate textureCoord

//Matrices
uniform mat4 mvpMatrix;
uniform mat4 projectionViewMatrix;

//Billboarding Data
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec2 billboardSize;
uniform vec3 centre;

//Mesh Data
in vec3 position;
in vec2 textureCoord;

//Data for the fragment shader
out vec2 frag_textureCoord;

void main() {
	//Pass the needed data over to the fragment shadr
	frag_textureCoord = textureCoord;
	
	//Assign the position of the current vertex
	gl_Position = projectionViewMatrix * vec4(centre + ((cameraRight * position.x * billboardSize.x) + (cameraUp * position.y * billboardSize.y)), 1.0);
}