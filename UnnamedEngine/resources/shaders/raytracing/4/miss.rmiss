#version 460
#extension GL_EXT_ray_tracing : enable

struct RayPayload {
	vec3 hitValue;
	int depth;
	vec3 attenuation;
};

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

void main() {
    rayPayload.hitValue = vec3(0.0, 0.0, 0.2) * rayPayload.attenuation;
}