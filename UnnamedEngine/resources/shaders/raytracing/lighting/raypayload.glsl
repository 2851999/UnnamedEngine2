struct RayPayload {
	vec3 hitValue;
	vec3 attenuation;
	int depth;
	int done;
	vec3 rayOrigin;
	vec3 rayDir;
};