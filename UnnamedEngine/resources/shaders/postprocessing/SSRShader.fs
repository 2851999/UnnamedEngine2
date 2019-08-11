#include "../Core.glsl"

layout(location = 0) in vec2 ue_frag_textureCoord;

#map uniform PositionBuffer ue_gPosition
#map uniform NormalBuffer ue_gNormal
#map uniform Texture0 ue_gAlbedo
#map uniform MetalnessAOBuffer ue_gMetalnessAO
#map uniform Texture1 ue_bloomTexture

/* Input data for the SSR */
uniform sampler2D ue_gPosition;
uniform sampler2D ue_gNormal;
uniform sampler2D ue_gAlbedo;
uniform sampler2D ue_gMetalnessAO;
uniform sampler2D ue_bloomTexture;

/* Various constants for the SSR */
const float stepValue = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 10;
const float reflectionSpecularFalloffExponent = 5;

const vec3 scale = vec3(0.8, 0.8, 0.8);
const float K = 19.19;

layout(location = 0) out vec4 ue_FragColour;

vec3 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth) {
	float depth;
	
	vec4 projectedCoord;
	for (int i = 0; i < numBinarySearchSteps; ++i) {
		projectedCoord = ue_projectionMatrix * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
		
		vec4 depthValue = texture(ue_gPosition, projectedCoord.xy).xyzw;
		//Value is initially in world space, so convert to view space
		depthValue = ue_viewMatrix * depthValue;
		//depthValue.xyz /= depthValue.w;
		//depthValue.xyz = depthValue.xyz * 0.5 + 0.5;
		depth = depthValue.z;
		
		dDepth = hitCoord.z - depth;
		
		dir *= 0.5;
		if (dDepth > 0.0)
			hitCoord += dir;
		else
			hitCoord -= dir;
	}
	
	projectedCoord = ue_projectionMatrix * vec4(hitCoord, 1.0);
	projectedCoord.xy /= projectedCoord.w;
	projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
	
	return vec3(projectedCoord.xy, depth);
}

vec4 rayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth) {
	dir *= stepValue;
	
	float depth;
	int steps;
	vec4 projectedCoord;
	
	for (int i = 0; i < maxSteps; ++i) {
		hitCoord += dir;
		
		projectedCoord = ue_projectionMatrix * vec4(hitCoord, 1.0);
		projectedCoord.xy /= projectedCoord.w;
		projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
		
		vec4 depthValue = texture(ue_gPosition, projectedCoord.xy).xyzw;
		//Value is initially in world space, so convert to view space
		depthValue = ue_viewMatrix * depthValue;
		//depthValue.xyz /= depthValue.w;
		//depthValue.xyz = depthValue.xyz * 0.5 + 0.5;
		depth = depthValue.z;
		
		if (depth > 1000.0)
			continue;
			
		dDepth = hitCoord.z - depth;
		
		if ((dir.z - dDepth) < 1.2 && dDepth <= 0.0)
			return vec4(binarySearch(dir, hitCoord, dDepth), 1.0);
		
		steps++;
	}
	
	return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 hash(vec3 a) {
	a = fract(a * scale);
	a += dot(a, a.yxz + K);
	return fract((a.xxy + a.yxx) * a.zyx);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
	vec3 fragPosition = texture(ue_gPosition, ue_frag_textureCoord).rgb;

	vec3 albedo = texture(ue_gAlbedo, ue_frag_textureCoord).rgb;
	
	vec4 normalRough = texture(ue_gNormal, ue_frag_textureCoord);
	vec3 normal = normalRough.rgb;
	float roughness = normalRough.a;

	vec3 metalnessAO = texture(ue_gMetalnessAO, ue_frag_textureCoord).rgb;
	float metalness = metalnessAO.r;
	float ao = metalnessAO.g;

	//mat4 test = ue_viewMatrix;
	vec3 viewPos = (ue_viewMatrix * vec4(fragPosition, 1.0)).xyz;
	//test[3] = vec4(0.0, 0.0, 0.0, 1.0);
	//Ignore translation for normal
	vec3 viewNormal = (ue_viewMatrix * vec4(normal, 0.0)).xyz;
	
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metalness);
	vec3 fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);
	
	vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));
	vec3 hitPos = viewPos;
	float dDepth;
	vec3 wp = fragPosition;
	vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), roughness); //SPECULAR INSTEAD OF 1??????
	vec4 coords = rayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth); //NOT VIEW POS
	
	vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
	float screenEdgeFactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
	float reflectionMultiplier = pow(metalness, reflectionSpecularFalloffExponent) * screenEdgeFactor * -reflected.z;
	
	vec3 colour = albedo + texture(ue_gAlbedo, coords.xy).rgb * clamp(reflectionMultiplier, 0.0, 0.9) * fresnel;
	colour += texture(ue_bloomTexture, ue_frag_textureCoord).rgb;
	
	//colour += texture(ue_gAlbedo, coords.xy).rgb;
	
	//ue_FragColour = vec4(vec3(coords.z), 1.0);
	ue_FragColour = vec4(colour, 1.0);
}