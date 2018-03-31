#include "Core.glsl"

#map uniform CameraPosition ue_cameraPosition

uniform vec3 ue_cameraPosition;

in vec3 ue_frag_position;
in vec2 ue_frag_textureCoord;
in vec3 ue_frag_normal;

out vec4 ue_FragColour;