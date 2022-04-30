#version 460
#extension GL_EXT_ray_tracing : enable

#include "raypayload.glsl"

layout(set = 1, binding = 14) uniform samplerCube skyboxTexture;

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

void main() {
    rayPayload.hitValue = texture(skyboxTexture, rayPayload.rayDirection).rgb;
    //rayPayload.hitValue = vec3(0.0, 0.0, 0.2);
}