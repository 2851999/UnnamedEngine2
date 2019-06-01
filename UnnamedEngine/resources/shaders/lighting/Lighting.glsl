#define MAX_LIGHTS 6

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

layout (std140, binding = 4) uniform UELightsData {
	uniform UELight ue_lights[MAX_LIGHTS];
	uniform mat4 ue_lightSpaceMatrix[MAX_LIGHTS];
	
	uniform vec4 ue_lightAmbient;
	uniform int ue_numLights;
};