#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#include "raypayload.glsl"
#include "../closesthit.glsl"

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

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

	const vec3 barycentrics = ueComputeBarycentrics();

	// //Compute the coordinates of the hit position
	// const vec3 position = ueFromBarycentric(v0.position, v1.position, v2.position, barycentrics);
	// const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));  //Transforming to world space

	// //Compute the normal at the hit position
	// const vec3 normal = ueFromBarycentric(v0.normal, v1.normal, v2.normal, barycentrics);
	// const vec3 worldNorm = normalize(vec3(normal * gl_WorldToObjectEXT));  //Transform to world space

	//Compute the texture coordinate at the hit position
	const vec2 textureCoord = ueFromBarycentric(v0.textureCoord, v1.textureCoord, v2.textureCoord, barycentrics);

	//Material of the object
	int matIndex = matIndices.i[gl_PrimitiveID + indexOffset];
	int textureIndex = matIndex + modelResource.textureOffset;
	UEMaterial mat = materials.m[matIndex];

	//Obtain the diffuse colour
	vec3 albedo = mat.diffuseColour.rgb;

	if (mat.hasDiffuseTexture)
		albedo *= texture(diffuseTextures[nonuniformEXT(textureIndex)], textureCoord).xyz;

  	rayPayload.hitValue = albedo;
}
