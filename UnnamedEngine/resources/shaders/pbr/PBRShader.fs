#include "../lighting/Lighting.fs"

#include "PBRCore.fs"

#map uniform IrradianceMap irradianceMap
#map uniform PrefilterMap prefilterMap
#map uniform BRDFLUT brdfLUT

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D   brdfLUT; 

out vec4 ue_FragColour;

void main() {
	vec3 albedo = ueGetMaterialDiffuse(ue_frag_textureCoord).rgb;
	vec3 normal = ueCalculateNormal(ue_frag_textureCoord);
	float metallic = ueGetMaterialAmbient(ue_frag_textureCoord).r;
	float roughness = ueGetMaterialShininess(ue_frag_textureCoord);
	float ao = ueGetMaterialSpecular(ue_frag_textureCoord).r;

	//Normal
    vec3 N = normalize(normal);
    //View direction
    vec3 V = normalize(ue_cameraPosition - ue_frag_position);

    vec3 R = reflect(-V, N); 

    //Surface reflection at 0 incidence (how much surface reflects looking directly at the surface)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < ue_numLights; i++) {
        if (ue_lights[i].type == 2) {
            //PER LIGHT STUFF ------------------------------------------------
            vec3 lightPosition = ue_lights[i].position;
            vec3 lightColor = ue_lights[i].diffuseColour;

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
        }
    }

    //vec3 ambient = vec3(0.03) * albedo * ao;

    //Ambient lighting
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 colour = ambient + Lo;

    colour = ueGammaCorrect(ueReinhardToneMapping(colour));

	ue_FragColour = vec4(colour, 1.0);
	//ue_FragColour = vec4(ue_frag_textureCoord, 0.0, 1.0);
}