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

layout (std140, binding = 4) uniform UELightingData {
	UELight ue_lights[MAX_LIGHTS];
	mat4 ue_lightSpaceMatrix[MAX_LIGHTS];
	
	vec4 ue_lightAmbient;
	int ue_numLights;
	
	bool ue_useEnvironmentMap;
};