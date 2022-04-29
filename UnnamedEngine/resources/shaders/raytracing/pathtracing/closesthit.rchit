#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "sampling.glsl"

#include "../closesthit.glsl"
#include "raypayload.glsl"

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

float schlick(float cosine, float refractionIndex) {
	float r0 = (1.0 - refractionIndex) / (1.0 + refractionIndex);
	r0 *= r0;
	return r0 + (1.0 - r0) * pow(1.0 - cosine, 5.0);
}

void main() {
	//Obtain the model data
	ModelData modelResource = modelData.i[gl_InstanceCustomIndexEXT];
	Vertices vertices = Vertices(modelResource.vertexBufferAddress);
	Indices indices = Indices(modelResource.indexBufferAddress);
	MatIndices matIndices = MatIndices(modelResource.matIndexBufferAddress);
	Materials materials = Materials(modelResource.matDataBufferAddress);
	OffsetIndices offsetIndices = OffsetIndices(modelResource.offsetIndicesBufferAddress);

	const uvec2 offsets = offsetIndices.i[gl_PrimitiveID];
	const uint indexOffset = offsets.x;
	const uint vertexOffset = offsets.y;

	//Indices of the triangle
	ivec3 ind = indices.i[gl_PrimitiveID + indexOffset];

	//Vertices of the triangle
	Vertex v0 = vertices.v[ind.x + vertexOffset];
	Vertex v1 = vertices.v[ind.y + vertexOffset];
	Vertex v2 = vertices.v[ind.z + vertexOffset];

	//Material of the object
	int matIndex = matIndices.i[gl_PrimitiveID + indexOffset];
	int textureIndex = matIndex + modelResource.textureOffset;
	UEMaterial mat = materials.m[matIndex];

	const vec3 barycentrics = ueComputeBarycentrics();

	//Computing the coordinates of the hit position
	const vec3 position = ueFromBarycentric(v0.position, v1.position, v2.position, barycentrics);
	const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));  //Transforming to world space

	const vec2 textureCoord = ueFromBarycentric(v0.textureCoord, v1.textureCoord, v2.textureCoord, barycentrics);

	//Computing the normal at the hit position
	vec3 normal = ueFromBarycentric(v0.normal, v1.normal, v2.normal, barycentrics);
	vec3 tangent = ueFromBarycentric(v0.tangent, v1.tangent, v2.tangent, barycentrics);
	vec3 bitangent = ueFromBarycentric(v0.bitangent, v1.bitangent, v2.bitangent, barycentrics);

	if (mat.hasNormalMap) {
		mat3 normalMatrix = transpose(inverse(mat3(gl_ObjectToWorldEXT)));
		vec3 T = normalize(normalMatrix * tangent);
		vec3 B = normalize(normalMatrix * bitangent);
		vec3 N = normalize(normal);
	
		mat3 tbnMatrix = mat3(-T, B, N);

		normal = texture(normalMaps[nonuniformEXT(textureIndex)], textureCoord).rgb;
		normal.y = 1 - normal.y;
		normal = normalize(normal * 2.0 - 1.0);

		normal = tbnMatrix * normal;
	}

	const vec3 worldNorm = normalize(vec3(normal * gl_WorldToObjectEXT));  //Transform to world space
	// const vec3 worldTangent = normalize(vec3(tangent * gl_WorldToObjectEXT));  //Transform to world space
	// const vec3 worldBitangent = normalize(vec3(bitangent * gl_WorldToObjectEXT));  //Transform to world space

	vec3 albedo = mat.diffuseColour.rgb;

	if (mat.hasDiffuseTexture)
		albedo *= texture(diffuseTextures[nonuniformEXT(textureIndex)], textureCoord).xyz;

	float metalness = mat.ambientColour.r;
	float roughness = mat.shininess;
	float ao = mat.specularColour.r;

	vec3 emittance = mat.emissiveColour.xyz;

	if (mat.hasEmissiveTexture)
		emittance *= texture(emissiveTextures[nonuniformEXT(textureIndex)], textureCoord).xyz;

	//emittance = mix(emittance, albedo, 0.01);

	//Tangent/bitangent above are different?!?!?!?!
	vec3 tangent2, bitangent2;
	createCoordinateSystem(worldNorm, tangent2, bitangent2);
	vec3 rayOrigin    = worldPos;
	vec3 rayDirection = samplingHemisphere(rayPayload.seed, tangent2, bitangent2, worldNorm);

	//Generate the next ray in a random direction
	// vec3 rayOrigin    = worldPos;
	// vec3 rayDirection = samplingHemisphere(rayPayload.seed, worldTangent, worldBitangent, worldNorm);

	//Probability of the new ray
  	const float p = 1.0 / SAMPLING_PI;

	//Compute the BRDF for this ray
  	float cos_theta = dot(rayDirection, worldNorm);
 	vec3  BRDF      = albedo / SAMPLING_PI;

	//Reflection
	// if (mat.shininess > 0.1) {
	// 	//Reflect
	// 	//rayDirection = reflect(rayPayload.rayDirection, worldNorm);

	// 	//Refract
	// 	float refractionIndex = 1.1;

	// 	float dirDotNorm = dot(rayPayload.rayDirection, worldNorm);

	// 	vec3 normalOut = dirDotNorm > 0 ? -worldNorm : worldNorm;
	// 	float nit = dirDotNorm > 0 ? refractionIndex : 1.0 / refractionIndex;
	// 	float cosine = dirDotNorm > 0 ? refractionIndex * dirDotNorm : -dirDotNorm;
	// 	vec3 refracted = refract(rayPayload.rayDirection, normalOut, nit);


	// 	float reflectionProb = refracted != vec3(0) ? schlick(cosine, refractionIndex) : 1;

	// 	if (rand(rayPayload.seed) < reflectionProb)
	// 		rayDirection = reflect(rayPayload.rayDirection, worldNorm);
	// 	else
	// 		rayDirection = refracted;

	// 	//Make clearer
	// 	BRDF *= 1.25;
	// }

	rayPayload.rayOrigin    = rayOrigin;
  	rayPayload.rayDirection = rayDirection;
  	rayPayload.hitValue     = emittance;
  	rayPayload.weight       = BRDF * cos_theta / p;

	//Stop if have hit a light source
	if (emittance.x > 0.0 || emittance.y > 0.0 || emittance.z > 0.0)
		rayPayload.depth = 1000;
}
