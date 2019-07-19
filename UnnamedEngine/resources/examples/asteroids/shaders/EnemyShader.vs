#version 420

#map uniform ViewProjectionMatrix vpMatrix
#map attribute Visible visible
#map attribute Position position
#map attribute Normal normal
#map attribute ModelMatrix modelMatrix
#map attribute NormalMatrix normalMatrix

uniform mat4 vpMatrix;

in float visible;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 normal;
in mat4 modelMatrix;
in mat3 normalMatrix;

out float frag_visible;
out vec3 frag_position;
out vec3 frag_normal;

void main() {
	frag_visible = visible;
	
	if (visible > 0.5) {
		frag_position = vec3(modelMatrix * vec4(position, 1.0));
		frag_normal = normalMatrix * normal;

		gl_Position = (vpMatrix * modelMatrix) * vec4(position, 1.0);
	}
}