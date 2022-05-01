#version 460
#extension GL_EXT_ray_tracing : enable

#include "raypayload.glsl"

layout(set = 1, binding = 14) uniform samplerCube skyboxTexture;

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

void main() {
	//Either take the usual miss colour if directly miss, otherwise
	//add a small contribution from the environment
	if(rayPayload.depth == 0)
		rayPayload.hitValue = texture(skyboxTexture, rayPayload.rayDirection).rgb;
		//rayPayload.hitValue = vec3(0.0, 0.0, 0.2);
	else
		rayPayload.hitValue = texture(skyboxTexture, rayPayload.rayDirection).rgb * (vec3(23.47, 21.31, 20.79) * 0.2);
	//Stop tracing as missed
	rayPayload.depth = 1000;
}