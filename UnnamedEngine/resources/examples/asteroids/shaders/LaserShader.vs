#version 140

#map uniform ViewProjectionMatrix vpMatrix
#map attribute Visible visible
#map attribute Position position
#map attribute ModelMatrix modelMatrix

uniform mat4 vpMatrix;

in float visible;

in vec3 position;
in mat4 modelMatrix;

out float frag_visible;
out vec3 frag_position;

void main() {
	frag_visible = visible;
	
	if (visible > 0.5) {
		gl_Position = (vpMatrix * modelMatrix) * vec4(position, 1.0);
	}
}