#version 460
#extension GL_EXT_ray_tracing : enable

#include "raypayload.glsl"

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

void main() {
	rayPayload.hitValue = vec3(0.0, 0.0, 0.2);
}