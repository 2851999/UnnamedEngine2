#include "Core.glsl"

#map uniform MVPMatrix mvpMatrix
#map uniform ModelMatrix modelMatrix
#map uniform NormalMatrix normalMatrix
#map attribute Position position
#map attribute TextureCoordinate textureCoord

#define UE_LOCATION_POSITION 0
#define UE_LOCATION_TEXTURE_COORD 1

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

layout(location = UE_LOCATION_POSITION) in vec3 position;
layout(location = UE_LOCATION_TEXTURE_COORD) in vec2 textureCoord;

out vec3 frag_position;
out vec2 frag_textureCoord;

void ue_assignPosition() {
	frag_position = vec3(modelMatrix * vec4(position, 1.0));
}

void ue_assignTextureCoord() {
	frag_textureCoord = textureCoord;
}

void ue_calculatePosition() {
	gl_Position = mvpMatrix * vec4(position, 1.0);
}