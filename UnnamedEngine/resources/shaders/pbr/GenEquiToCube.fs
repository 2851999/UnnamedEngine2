#version 420

layout(set = 0, binding = 0) uniform sampler2D equirectangularMap;

layout(location = 0) in vec4 localPos;

layout(location = 0) out vec4 FragColor;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = sampleSphericalMap(normalize(localPos.xyz));
    vec3 colour = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(colour, 1.0);
}