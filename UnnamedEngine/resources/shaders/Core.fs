#include "Core.glsl"

#map uniform Camera_Position ue_camera_position

uniform vec3 ue_camera_position;

in vec3 ue_frag_position;
in vec2 ue_frag_textureCoord;
in vec3 ue_frag_normal;

out vec4 ue_FragColour;