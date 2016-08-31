#version 140

#map uniform MVPMatrix mvpMatrix
#map uniform Camera_Right cameraRight
#map uniform Camera_Up cameraUp
#map attribute Position position
#map attribute PositionsData positionsData
#map attribute Colour colour
#map attribute TextureData textureData

uniform mat4 mvpMatrix;

uniform vec3 cameraRight;
uniform vec3 cameraUp;

in vec3 position;
in vec4 positionsData;
in vec4 colour;
in vec4 textureData;

out vec4 frag_colour;
out vec2 frag_textureCoord;

void main() {
	vec3 centre = positionsData.xyz;
	float billboardSize = positionsData.w;
	
	frag_colour = colour;
	
	float top = textureData.x;
	float left = textureData.y;
	float bottom = textureData.z;
	float right = textureData.w;
	
	frag_textureCoord = vec2(((position.x + 0.5) * right) - ((position.x - 0.5) * left), ((position.y + 0.5) * top) - ((position.y - 0.5) * bottom));
	
	gl_Position = mvpMatrix * vec4(centre + ((cameraRight * position.x * billboardSize) + (cameraUp * position.y * billboardSize)), 1.0);
}