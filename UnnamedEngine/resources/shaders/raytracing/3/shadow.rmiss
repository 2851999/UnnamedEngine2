#version 460
#extension GL_EXT_ray_tracing : enable

//Hit payload for shadow rays
layout(location = 1) rayPayloadInEXT bool isShadowed;

void main() {
    isShadowed = false;
}