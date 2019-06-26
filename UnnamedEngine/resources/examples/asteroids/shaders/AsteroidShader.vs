#version 420

#map uniform ViewProjectionMatrix vpMatrix
#map uniform UseNormalMap useNormalMap
#map attribute Visible visible
#map attribute Position position
#map attribute TextureCoordinate textureCoord
#map attribute Normal normal
#map attribute Tangent tangent
#map attribute Bitangent bitangent
#map attribute ModelMatrix modelMatrix
#map attribute NormalMatrix normalMatrix

uniform mat4 vpMatrix;
uniform bool useNormalMap;

in float visible;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoord;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
in mat4 modelMatrix;
in mat3 normalMatrix;

out float frag_visible;
out vec3 frag_position;
out vec2 frag_textureCoord;
out vec3 frag_normal;

out mat3 frag_tbnMatrix;

void main() {
	frag_visible = visible;
	
	if (visible > 0.5) {
		frag_position = vec3(modelMatrix * vec4(position, 1.0));
		frag_textureCoord = textureCoord;
		frag_normal = normalMatrix * normal;

		gl_Position = (vpMatrix * modelMatrix) * vec4(position, 1.0);
		
		if (useNormalMap) {
			vec3 T = normalize(normalMatrix * tangent);
			vec3 B = normalize(normalMatrix * bitangent);
			vec3 N = normalize(frag_normal);
		
			frag_tbnMatrix = mat3(T, B, N);
		}
	}
}