#version 460
#extension GL_EXT_ray_tracing : enable

#include "raypayload.glsl"

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

void main() {
	//Either take the usual miss colour if directly miss, otherwise
	//add a small contribution from the environment
	if(rayPayload.depth == 0)
		rayPayload.hitValue = vec3(0.0, 0.0, 0.2);
	else
		rayPayload.hitValue = vec3(0.01);
	//Stop tracing as missed
	rayPayload.depth = 1000;
}