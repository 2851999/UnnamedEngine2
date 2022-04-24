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

struct RayPayload {
	vec3 hitValue;
	vec3 attenuation;
	int depth;
	int done;
	vec3 rayOrigin;
	vec3 rayDir;
};

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

//Hit payload for shadow rays
layout(location = 1) rayPayloadEXT bool isShadowed;

hitAttributeEXT vec2 attribs;

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };   //Positions of a model
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };     //Triangle indices
layout(buffer_reference, scalar) buffer MatIndices { int i[]; };       //Material index for each triangle
layout(buffer_reference, scalar) buffer Materials { UEMaterial m[]; }; //Array of all materials on a model
layout(buffer_reference, scalar) buffer OffsetIndices { uvec2 i[]; };

layout(set = 1, binding = 0) uniform accelerationStructureEXT tlas;
layout(set = 1, binding = 22, scalar) buffer ModelData_ { ModelData i[]; } modelData;


//vec3 lightPosition = vec3(0.0, 1.0, 0.0);
vec3 lightPosition = vec3(0.20607, 0.799975, 1.32652);
float lightIntensity = 1;
float lightDistance = 100;
uint lightType = 0;

vec3 computeDiffuse(UEMaterial mat, vec3 L, vec3 normal) {
	float dotNL = max(dot(normal, L), 0.0);
	vec3 c = mat.diffuseColour.rgb * dotNL;
	return c;
}

vec3 computeSpecular(UEMaterial mat, vec3 viewDir, vec3 L, vec3 normal) {
	const float kPi        = 3.14159265;
	const float kShininess = max(mat.shininess, 4.0);

	const float kEnergyConservation = (2.0 + kShininess) / (2.0 * kPi);
	vec3        V                   = normalize(-viewDir);
	vec3        R                   = reflect(-L, normal);
	float       specular            = kEnergyConservation * pow(max(dot(V, R), 0.0), kShininess);

	return vec3(mat.specularColour * specular);
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

	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

	//Computing the coordinates of the hit position
	const vec3 position = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
	const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));  //Transforming to world space

	//Computing the normal at the hit position
	const vec3 normal = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
	const vec3 worldNorm = normalize(vec3(normal * gl_WorldToObjectEXT));  //Transforming to world space

	//Material of the object
	int matIndex = matIndices.i[gl_PrimitiveID + indexOffset];
	UEMaterial mat = materials.m[matIndex];

	//Simple lighting
	vec3 L;
	//Point light
	if (lightType == 0) {
		vec3 lightDirection = lightPosition - worldPos;
		lightDistance = length(lightDirection);
		lightIntensity = lightIntensity / (lightDistance * lightDistance);
		L = normalize(lightDirection);
	//Directional light
	} else {
		L = normalize(lightPosition);
	}

	vec3 diffuse = computeDiffuse(mat, L, worldNorm);

	vec3 specular = vec3(0.0);
	float attenuation = 1.0;

	//Trace shadow rays only if the light is visible from the surface (light is infront of it)
	if (dot(worldNorm, L) > 0.0) {
		float tMin = 0.001;
		float tMax = lightDistance;
		vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
		vec3 rayDir = L;
		uint rayFlags = gl_RayFlagsTerminateOnFirstHitEXT     //Don't invoke hit shader
						| gl_RayFlagsOpaqueEXT                //As not invoking hit shader, treat all objects as opaque
						| gl_RayFlagsSkipClosestHitShaderEXT; //Stop after first hit

		//Assume shadowed unless shadow miss shader invoked
		isShadowed = true;

		//Trace a ray
		traceRayEXT(tlas,          //TLAS
					rayFlags,      //rayFlags
					0xFF,          //culling mask - binary and with instance mask and skips intersection if result is 0 (currently using 0xFF in generation as well
					0,             //sbtRecordOffset
					0,             //sbtRecordStride
					1,             //missIndex - 1 now since shadow miss shader has index 1
					origin.xyz,    //ray origin
					tMin,          //min range
					rayDir,        //direction
					tMax,          //max range
					1              //payload location = 1 (isShadowed)
		);

		if (isShadowed) {
			attenuation = 0.3;
		} else {
			specular = computeSpecular(mat, gl_WorldRayDirectionEXT, L, worldNorm);
		}
	}

	//Check if material is reflective
	if (mat.shininess > 0.1) {
		//Request another reflection
		vec3 origin = worldPos;
		vec3 rayDir = reflect(gl_WorldRayDirectionEXT, worldNorm);
		rayPayload.attenuation *= mat.specularColour.rgb;
		rayPayload.done = 0;
		rayPayload.rayOrigin = origin;
		rayPayload.rayDir = rayDir;
	}

	//Normals wont display if negative so check using *-1!!!
	
	//rayPayload.hitValue = vec3(worldPos);
	//rayPayload.hitValue = vec3(vertexOffset / 8);
	//rayPayload.hitValue = vec3(gl_PrimitiveID / 8.0);
	//rayPayload.hitValue = vec3(mat.diffuseColour);
	//rayPayload.hitValue = vec3(isShadowed);
	rayPayload.hitValue = vec3(lightIntensity * attenuation * (diffuse + specular));
	//rayPayload.hitValue = vec3(gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT);
}
