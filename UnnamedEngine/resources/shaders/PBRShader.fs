#include "Lighting.fs"

#map uniform Albedo albedoMap
#map uniform Metallic metallicMap
#map uniform Roughness roughnessMap
#map uniform AO aoMap
#map uniform IrradianceMap irradianceMap
#map uniform PrefilterMap prefilterMap
#map uniform BRDFLUT brdfLUT

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT; 

out vec4 ue_FragColour;

const float PI = 3.14159265359;

//Returns ratio of light that gets reflected on a surface (k_S)
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
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

void main() {
	vec3 lightPosition = vec3(0.5, 0.5, 2.0);
	vec3 lightColor = vec3(23.47, 21.31, 20.79);

	vec3 albedo = texture(albedoMap, ue_frag_textureCoord).rgb;
	vec3 normal = ueCalculateNormal(ue_frag_textureCoord);
	float metallic = texture(metallicMap, ue_frag_textureCoord).r;
	float roughness = texture(roughnessMap, ue_frag_textureCoord).r;
	float ao = texture(aoMap, ue_frag_textureCoord).r;

	//Normal
    vec3 N = normalize(normal);
    //View direction
    vec3 V = normalize(ue_cameraPosition - ue_frag_position);

    vec3 R = reflect(-V, N); 

    //Surface reflection at 0 incidence (how much surface reflects looking directly at the surface)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    //PER LIGHT STUFF ------------------------------------------------

    //Calculate radiance
    vec3 L = normalize(lightPosition - ue_frag_position);
    vec3 H = normalize(V + L);
    float distance = length(lightPosition - ue_frag_position);
    float attenuation = 1 / (distance * distance); //Inverse square - need gamma correction
    vec3 radiance = lightColor * attenuation;

    //Cook-torrance brdf
    float NDF = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS; //Ratio of refraction

    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;

    //----------------------------------------------------------------

    //vec3 ambient = vec3(0.03) * albedo * ao;

    //Ambient lighting
    vec3 F2 = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS2 = F2;
    vec3 kD2 = 1.0 - kS2;
    kD2 *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular2 = prefilteredColor * (F2 * brdf.x + brdf.y);

    vec3 ambient = (kD2 * diffuse + specular2) * ao;

    vec3 colour = ambient + Lo;

    colour = ueGammaCorrect(ueReinhardToneMapping(colour));

	ue_FragColour = vec4(colour, 1.0);
	//ue_FragColour = vec4(ue_frag_textureCoord, 0.0, 1.0);
}