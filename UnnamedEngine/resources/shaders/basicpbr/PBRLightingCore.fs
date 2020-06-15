#include "../lighting/Lighting.fs"

#include "PBRCore.fs"

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
vec3 ueGetLightingPBR(vec3 normal, vec3 fragPos, vec3 albedo, float metalness, float roughness, float ao, vec4 fragPosLightSpace[MAX_LIGHTS]) {
    //View direction
    vec3 V = normalize(ue_cameraPosition.xyz - fragPos);

    vec3 R = reflect(-V, normal); 

    //Surface reflection at 0 incidence (how much surface reflects looking directly at the surface)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalness);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < ue_numLights; ++i) {
        if (ue_lights[i].type == 1) {
            if (ue_lights[i].useShadowMap)
                Lo += ueCalculateDirectionalLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0) * (1.0 - ueCalculateShadow(ue_lights[i], ue_lightTexturesShadowMap[i], fragPosLightSpace[i], normal));
            else
                Lo += ueCalculateDirectionalLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
        } else if (ue_lights[i].type == 2) {
			if (ue_lights[i].useShadowMap)
				Lo += ueCalculatePointLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0) * (1.0 - ueCalculatePointShadow(ue_lights[i], ue_lightTexturesShadowCubemap[i], fragPos, ue_cameraPosition.xyz));
			else
				Lo += ueCalculatePointLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
        } else if (ue_lights[i].type == 3)
			if (ue_lights[i].useShadowMap)
				Lo += ueCalculateSpotLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0) * (1.0 - ueCalculateShadow(ue_lights[i], ue_lightTexturesShadowMap[i], fragPosLightSpace[i], normal));
			else
				Lo += ueCalculateSpotLightPBR(ue_lights[i], normal, V, fragPos, albedo, metalness, roughness, F0);
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    if (ue_lightAmbient.r == 0.0)
        ambient *= 0.0;

    return ambient + Lo;
}