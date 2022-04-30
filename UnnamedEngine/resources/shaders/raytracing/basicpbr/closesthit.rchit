#version 460
#extension GL_EXT_ray_tracing : enable
#extension GL_EXT_nonuniform_qualifier : enable

#extension GL_EXT_scalar_block_layout : enable
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#include "../closesthit.glsl"
#include "raypayload.glsl"

#include "../../basicpbr/PBRCore.fs"

#define MAX_LIGHTS 20

struct UELight {
	int  type;
	
	vec4 position;
	vec4 direction;
	vec4 diffuseColour;
	vec4 specularColour;
	
	float constant;
	float linear;
	float quadratic;
	
	float innerCutoff;
	float outerCutoff;
	
	bool useShadowMap;
};

layout(std140, set = 1, binding = 3) uniform UELightData {
	UELight ue_lights[MAX_LIGHTS];
	
	vec4 ue_lightAmbient;
	int ue_numLights;
};

//Hit payload
layout(location = 0) rayPayloadInEXT RayPayload rayPayload;

//Hit payload for shadow rays
layout(location = 1) rayPayloadEXT bool isShadowed;

vec3 ueCalculateLightPBR(UELight light, vec3 lightDirection, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightColor = light.diffuseColour.xyz;

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

	//Trace shadow rays only if the light is visible from the surface (light is infront of it)
	if (NdotL > 0.0) {
		float tMin = 0.001;
		float tMax = 100;
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
			radiance *= 0.2;
		}
	}

    return  (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 ueCalculateDirectionalLightPBR(UELight light, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightDirection = normalize(-light.direction.xyz);

    return ueCalculateLightPBR(light, lightDirection, normal, viewDirection, fragPos, albedo, metalness, roughness, F0);
}

vec3 ueCalculatePointLightPBR(UELight light, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightPosition = light.position.xyz;

    vec3 lightDirection = normalize(lightPosition - fragPos);

    vec3 radiance = ueCalculateLightPBR(light, lightDirection, normal, viewDirection, fragPos, albedo, metalness, roughness, F0);

    float distanceToLight = length(lightPosition - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight); //Inverse square - need gamma correction

    radiance *= attenuation;

    return radiance;
}

vec3 ueCalculateSpotLightPBR(UELight light, vec3 normal, vec3 viewDirection, vec3 fragPos, vec3 albedo, float metalness, float roughness, vec3 F0) {
    vec3 lightPosition = light.position.xyz;

    //Calculate radiance
    vec3 lightDirection = normalize(lightPosition - fragPos);

    float theta = dot(lightDirection, normalize(-light.direction.xyz));

    if (theta > light.outerCutoff) {
        float e = light.innerCutoff - light.outerCutoff;
        float intensity = clamp((theta - light.outerCutoff) / e, 0.0, 1.0);

        //Attenuation
        float distanceToLight = length(lightPosition - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distanceToLight + light.quadratic * distanceToLight * distanceToLight); //Inverse square - need gamma correction

        vec3 radiance = ueCalculateLightPBR(light, lightDirection, normal, viewDirection, fragPos, albedo, metalness, roughness, F0);
        radiance *= intensity * attenuation;

        return radiance;
    } else {
        return vec3(0.0);
    }
}

//Returns the result of applying all lighting calculations
vec3 ueGetLightingPBR(vec3 normal, vec3 fragPos, vec3 albedo, float metalness, float roughness, float ao) {
    //View direction
    vec3 V = normalize(-gl_WorldRayDirectionEXT); //Don't need frag pos anymore, used to be ue_cameraPosition.xyz - fragPos

    vec3 R = reflect(-V, normal); 

    //Surface reflection at 0 incidence (how much surface reflects looking directly at the surface)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalness);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < ue_numLights; ++i) {
        if (ue_lights[i].type == 1) {
            Lo += ueCalculateDirectionalLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
        } else if (ue_lights[i].type == 2) {
			Lo += ueCalculatePointLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
        } else if (ue_lights[i].type == 3)
			Lo += ueCalculateSpotLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
    }

    vec3 ambient = vec3(0.0);
    if (ue_lightAmbient.r > 0.0)
        ambient = vec3(ue_lightAmbient) * albedo * ao; //Used to use vec3(0.03) * albedo * ao

	//Check if material is reflective
	if (metalness > 0.05) {
		//Request another reflection
		vec3 origin = fragPos;
		vec3 rayDir = reflect(gl_WorldRayDirectionEXT, normal);
		rayPayload.done = 0;
		rayPayload.rayOrigin = origin;
		rayPayload.rayDirection = rayDir;
	}

    return ambient + Lo;
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
	vec3 normal = ueFromBarycentric(v0.normal, v1.normal, v2.normal, barycentrics);;
	vec3 tangent = ueFromBarycentric(v0.tangent, v1.tangent, v2.tangent, barycentrics);;
	vec3 bitangent = ueFromBarycentric(v0.bitangent, v1.bitangent, v2.bitangent, barycentrics);
	
	if (mat.hasNormalMap) {
		mat3 normalMatrix = transpose(inverse(mat3(gl_ObjectToWorldEXT)));
		vec3 T = normalize(normalMatrix * tangent);
		vec3 B = normalize(normalMatrix * bitangent);
		vec3 N = normalize(normal);
	
		mat3 tbnMatrix = mat3(-T, B, N);

		normal = tbnMatrix * ueGetMaterialNormal(textureIndex, textureCoord);
	}

	const vec3 worldNorm = normalize(vec3(normal * gl_WorldToObjectEXT));  //Transforming to world space

	vec3 albedo = ueGetMaterialDiffuse(mat, textureIndex, textureCoord).rgb;
	float metalness = ueGetMaterialAmbient(mat, textureIndex, textureCoord).r;
	float roughness = ueGetMaterialShininess(mat, textureIndex, textureCoord);
	float ao = ueGetMaterialSpecular(mat, textureIndex, textureCoord).r;




	//Normals wont display if negative so check using *-1!!!
	
	//rayPayload.hitValue = vec3(worldPos);
	//rayPayload.hitValue = vec3(vertexOffset / 8);
	//rayPayload.hitValue = vec3(gl_PrimitiveID / 8.0);
	//rayPayload.hitValue = vec3(mat.diffuseColour);
	//rayPayload.hitValue = vec3(isShadowed);
	//rayPayload.hitValue = vec3(lightIntensity * attenuation * (diffuse + specular));
	//rayPayload.hitValue = normal;
	rayPayload.hitValue = ueGetLightingPBR(worldNorm, worldPos, albedo, metalness, roughness, ao);
	//rayPayload.hitValue = vec3(gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT);
}
