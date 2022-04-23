#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

struct ModelData {
	uint64_t vertexBufferAddress;
	uint64_t indexBufferAddress;
	uint64_t matIndexBufferAddress;
	uint64_t matDataBufferAddress;
  uint64_t offsetIndicesBufferAddress;
};

struct Vertex {
  vec3 position;
  vec2 textureCoord;
  vec3 normal;
  vec3 tangent;
  vec3 bitangent;
};

struct UEMaterial {
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;
	
	bool hasAmbientTexture;
	bool hasDiffuseTexture;
	bool diffuseTextureSRGB;
	bool hasSpecularTexture;
	bool hasShininessTexture;
	bool hasNormalMap;
	bool hasParallaxMap;
	
	float parallaxScale;
	float shininess;
};

//Hit payload
layout(location = 0) rayPayloadInEXT vec3 hitValue;
hitAttributeEXT vec2 attribs;

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };      //Positions of a model
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };        //Triangle indices
layout(buffer_reference, scalar) buffer MatIndices { int i[]; };       //Material index for each triangle
layout(buffer_reference, scalar) buffer Materials { UEMaterial m[]; }; //Array of all materials on a model
layout(buffer_reference, scalar) buffer OffsetIndices { uvec2 i[]; };

layout(set = 1, binding = 22, scalar) buffer ModelData_ { ModelData i[]; } modelData;

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
  ivec3 ind = indices.i[gl_PrimitiveID];

  //Vertices of the triangle
  Vertex v0 = vertices.v[ind.x];
  Vertex v1 = vertices.v[ind.y];
  Vertex v2 = vertices.v[ind.z];

  const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

  //Computing the coordinates of the hit position
  const vec3 position = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
  const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));  //Transforming to world space

  //Computing the normal at the hit position
  const vec3 normal = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
  const vec3 worldNrm = normalize(vec3(normal * gl_WorldToObjectEXT));  //Transforming to world space

  //Material of the object
  int matIndex = matIndices.i[gl_PrimitiveID];
  UEMaterial mat = materials.m[matIndex];
  
  //hitValue = vec3(worldPos);
  //hitValue = vec3(worldNrm);
  hitValue = vec3(materials.m[0].diffuseColour);
  //hitValue = vec3(gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT);
}
