#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_GOOGLE_include_directive : enable

#include "sampling.glsl"

struct ModelData {
	uint64_t vertexBufferAddress;
	uint64_t indexBufferAddress;
	uint64_t matIndexBufferAddress;
	uint64_t matDataBufferAddress;
  	uint64_t offsetIndicesBufferAddress;
	int textureOffset;
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
	vec4 emissiveColour;
	
	bool hasAmbientTexture;
	bool hasDiffuseTexture;
	bool diffuseTextureSRGB;
	bool hasSpecularTexture;
	bool hasShininessTexture;
	bool hasNormalMap;
	bool hasParallaxMap;
	bool hasEmissiveTexture;
	
	float parallaxScale;
	float shininess;
};

struct RayPayload {
	vec3 hitValue;
	uint seed;
	uint depth;
	vec3 rayOrigin;
	vec3 rayDirection;
	vec3 weight;
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
layout(set = 1, binding = 2) uniform sampler2D ambientTextures[];
layout(set = 1, binding = 3) uniform sampler2D diffuseTextures[];
layout(set = 1, binding = 4) uniform sampler2D specularTextures[];
layout(set = 1, binding = 5) uniform sampler2D shininessTextures[];
layout(set = 1, binding = 6) uniform sampler2D normalMaps[];
layout(set = 1, binding = 7) uniform sampler2D parallaxMaps[];
layout(set = 1, binding = 8) uniform sampler2D emissiveTextures[];
layout(set = 1, binding = 22, scalar) buffer ModelData_ { ModelData i[]; } modelData;

layout(push_constant) uniform PushConstants {
	int frame;
} pushConstants;


//vec3 lightPosition = vec3(0.0, 1.0, 0.0);
vec3 lightPosition = vec3(0.20607, 0.799975, 1.32652);
float lightIntensity = 1;
float lightDistance = 100;
uint lightType = 0;

float lightConstant  = 0.0;
float lightLinear    = 0.0;
float lightQuadratic = 1.0;
vec3 lightDiffuseColour = vec3(23.47, 21.31, 20.79);
vec3 ue_lightAmbient = vec3(0.03, 0.03, 0.03);

const float PI = 3.14159265359;

//Wont work on all drivers
float radicalInverse_VdC(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 hammersley(uint i, uint N) {
	return vec2(float(i) / float(N), radicalInverse_VdC(i));
}

vec3 importanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    //Spherical coordinates to cartesian
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    //Tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float geometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

//Returns ratio of light that gets reflected on a surface (k_S)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

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

vec3 ueCalculateLightPBR(vec3 lightDirection, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightColor = lightDiffuseColour.xyz;

    //Calculate radiance
    vec3 L = lightDirection;
    vec3 H = normalize(viewDirection + L);
    vec3 radiance = lightColor;

    //Cook-torrance brdf
    float NDF = distributionGGX(normal, H, roughness);
    float G = geometrySmith(normal, viewDirection, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, viewDirection), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS; //Ratio of refraction

    kD *= 1.0 - metalness;

    float NdotL = max(dot(normal, L), 0.0);

    return  (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 ueCalculatePointLightPBR(vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightDirection = normalize(lightPosition - fragPos);

    vec3 radiance = ueCalculateLightPBR(lightDirection, normal, viewDirection, fragPos, albedo, metalness, roughness, F0);

    float distanceToLight = length(lightPosition - fragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * distanceToLight + lightQuadratic * distanceToLight * distanceToLight); //Inverse square - need gamma correction

    radiance *= attenuation;

	//Trace shadow rays only if the light is visible from the surface (light is infront of it)
	if (dot(normal, lightDirection) > 0.0) {
		float tMin = 0.001;
		float tMax = lightDistance;
		vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
		vec3 rayDir = lightDirection;
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
			radiance *= 0.3;
		}
	}

    return radiance;
}

//Returns the result of applying all lighting calculations
vec3 ueGetLightingPBR(vec3 normal, vec3 fragPos, vec3 albedo, float metalness, float roughness, float ao) {
    //View direction
    vec3 V = normalize(gl_WorldRayDirectionEXT - fragPos);

    vec3 R = reflect(-V, normal); 

    //Surface reflection at 0 incidence (how much surface reflects looking directly at the surface)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalness);

    vec3 Lo = ueCalculatePointLightPBR(normal, V, fragPos, albedo, metalness, roughness, F0);

    vec3 ambient = vec3(0.0);
    if (ue_lightAmbient.r > 0.0)
        ambient = vec3(ue_lightAmbient) * albedo * ao; //Used to use vec3(0.03) * albedo * ao

	vec3 F = fresnelSchlickRoughness(max(dot(normal, V), 0.0), F0, roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	kD *= metalness;

	// if (rayPayload.depth == 1)
	// 	rayPayload.attenuation = kD;

	// //Check if material is reflective
	// if (metalness > 0.0) {
	// 	//Request another reflection
	// 	vec3 origin = fragPos;
	// 	vec3 rayDir = reflect(gl_WorldRayDirectionEXT, normal);
	// 	rayPayload.done = 0;
	// 	rayPayload.rayOrigin = origin;
	// 	rayPayload.rayDir = rayDir;
	// }

    return ambient + Lo;
}

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

	const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

	//Computing the coordinates of the hit position
	const vec3 position = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
	const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(position, 1.0));  //Transforming to world space

	//Computing the normal at the hit position
	const vec3 normal = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
	const vec3 worldNorm = normalize(vec3(normal * gl_WorldToObjectEXT));  //Transforming to world space

	const vec2 textureCoord = v0.textureCoord * barycentrics.x + v1.textureCoord * barycentrics.y + v2.textureCoord * barycentrics.z;

	//Material of the object
	int matIndex = matIndices.i[gl_PrimitiveID + indexOffset];
	int textureIndex = matIndex + modelResource.textureOffset;
	UEMaterial mat = materials.m[matIndex];

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

	//Generate the next ray in a random direction
	vec3 tangent, bitangent;
	createCoordinateSystem(worldNorm, tangent, bitangent);
	vec3 rayOrigin    = worldPos;
	vec3 rayDirection = samplingHemisphere(rayPayload.seed, tangent, bitangent, worldNorm);

	//Probability of the new ray
  	const float p = 1.0 / SAMPLING_PI;

	//Compute the BRDF for this ray
  	float cos_theta = dot(rayDirection, worldNorm);
 	vec3  BRDF      = albedo / SAMPLING_PI;


	///////////////// VERY SKETCHY TEST

	// if (rayPayload.depth > 1) {
	// 	//View direction
	// 	vec3 viewDirection = normalize(rayPayload.rayOrigin.xyz - worldPos);

	// 	vec3 R = reflect(-viewDirection, worldNorm); 

	// 	//Surface reflection at 0 incidence (how much surface reflects looking directly at the surface)
	// 	vec3 F0 = vec3(0.04);
	// 	F0 = mix(F0, albedo, metalness);

	// 	vec3 Lo = vec3(0.0);

	// 	vec3 lightColor = rayPayload.hitValue;

	// 	//Calculate radiance
	// 	vec3 L = rayPayload.rayDirection;
	// 	vec3 H = normalize(viewDirection + L);
	// 	vec3 radiance = lightColor;

	// 	//Cook-torrance brdf
	// 	float NDF = distributionGGX(worldNorm, H, roughness);
	// 	float G = geometrySmith(worldNorm, viewDirection, L, roughness);
	// 	vec3 F = fresnelSchlick(max(dot(H, viewDirection), 0.0), F0);

	// 	vec3 numerator = NDF * G * F;
	// 	float denominator = 4.0 * max(dot(worldNorm, viewDirection), 0.0) * max(dot(-worldNorm, L), 0.0);
	// 	vec3 specular = numerator / max(denominator, 0.001);

	// 	vec3 kS = F;
	// 	vec3 kD = vec3(1.0) - kS; //Ratio of refraction

	// 	kD *= 1.0 - metalness;

	// 	float NdotL = max(dot(-worldNorm, L), 0.0);

	// 	BRDF = (kD * albedo / PI + specular) * radiance * NdotL;
	// }

	///////////////////////////////////

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
}
