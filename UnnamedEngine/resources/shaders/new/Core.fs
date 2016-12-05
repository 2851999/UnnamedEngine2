#include "Core.glsl"

#map uniform Camera_Position camera_position

uniform vec3 camera_position;

in vec3 frag_position;
in vec2 frag_textureCoord;

out vec4 FragColour;