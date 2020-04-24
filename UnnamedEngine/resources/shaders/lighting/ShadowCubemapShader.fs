#version 420

#include "ShadowCubemapShader.glsl"

layout(location = 0) in vec4 FragPos;

//layout(location = 1) out vec3 color;

void main() {
    float lightDistance = length(FragPos.xyz - lightPos.xyz);
	
	float farPlane = 25.0f;
	
	lightDistance = lightDistance / farPlane;
	
	gl_FragDepth = lightDistance;
}