#version 330

#map uniform LightPosition lightPos

uniform vec3 lightPos;

in vec4 FragPos;

out vec3 color;

void main() {
    float lightDistance = length(FragPos.xyz - lightPos);
	
	float farPlane = 25.0f;
	
	lightDistance = lightDistance / farPlane;
	
	gl_FragDepth = lightDistance;
}