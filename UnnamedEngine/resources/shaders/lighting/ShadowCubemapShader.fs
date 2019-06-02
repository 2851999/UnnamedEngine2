#version 420

#include "ShadowCubemapShader.glsl"

in vec4 FragPos;

out vec3 color;

void main() {
    float lightDistance = length(FragPos.xyz - lightPos.xyz);
	
	float farPlane = 25.0f;
	
	lightDistance = lightDistance / farPlane;
	
	gl_FragDepth = lightDistance;
}