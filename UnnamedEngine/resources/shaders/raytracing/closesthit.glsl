struct ModelData {
	uint64_t vertexBufferAddress;
	uint64_t indexBufferAddress;
	uint64_t matIndexBufferAddress;
	uint64_t matDataBufferAddress;
  	uint64_t offsetIndicesBufferAddress;
	int textureOffset;
};

struct Vertex {
  vec3 position;
  vec2 textureCoord;
  vec3 normal;
  vec3 tangent;
  vec3 bitangent;
};

struct UEMaterial {
	vec4 ambientColour;
	vec4 diffuseColour;
	vec4 specularColour;
	vec4 emissiveColour;
	
	bool hasAmbientTexture;
	bool hasDiffuseTexture;
	bool diffuseTextureSRGB;
	bool hasSpecularTexture;
	bool hasShininessTexture;
	bool hasNormalMap;
	bool hasParallaxMap;
	bool hasEmissiveTexture;
	
	float parallaxScale;
	float shininess;
};

layout(buffer_reference, scalar) buffer Vertices { Vertex v[]; };      //Positions of a model
layout(buffer_reference, scalar) buffer Indices { ivec3 i[]; };        //Triangle indices
layout(buffer_reference, scalar) buffer MatIndices { int i[]; };       //Material index for each triangle
layout(buffer_reference, scalar) buffer Materials { UEMaterial m[]; }; //Array of all materials on a model
layout(buffer_reference, scalar) buffer OffsetIndices { uvec2 i[]; };

layout(set = 1, binding = 0) uniform accelerationStructureEXT tlas;
layout(set = 1, binding = 2) uniform sampler2D ambientTextures[];
layout(set = 1, binding = 3) uniform sampler2D diffuseTextures[];
layout(set = 1, binding = 4) uniform sampler2D specularTextures[];
layout(set = 1, binding = 5) uniform sampler2D shininessTextures[];
layout(set = 1, binding = 6) uniform sampler2D normalMaps[];
layout(set = 1, binding = 7) uniform sampler2D parallaxMaps[];
layout(set = 1, binding = 8) uniform sampler2D emissiveTextures[];
layout(set = 1, binding = 22, scalar) buffer ModelData_ { ModelData i[]; } modelData;

layout(push_constant) uniform PushConstants {
	int frame;
} pushConstants;

hitAttributeEXT vec2 attribs;

vec3 ueComputeBarycentrics() {
	return vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
}

vec2 ueFromBarycentric(vec2 vector1, vec2 vector2, vec2 vector3, vec3 barycentrics) {
	return vector1 * barycentrics.x + vector2 * barycentrics.y + vector3 * barycentrics.z;
}

vec3 ueFromBarycentric(vec3 vector1, vec3 vector2, vec3 vector3, vec3 barycentrics) {
	return vector1 * barycentrics.x + vector2 * barycentrics.y + vector3 * barycentrics.z;
}

/* Various methods to get colours */
vec3 ueGetMaterialAmbient(UEMaterial material, int textureIndex, vec2 textureCoord) {
	vec3 ambientColour = vec3(material.ambientColour);
	if (material.hasAmbientTexture)
		ambientColour *= texture(ambientTextures[nonuniformEXT(textureIndex)], textureCoord).rgb;
	return ambientColour;
}

vec4 ueGetMaterialDiffuse(UEMaterial material, int textureIndex, vec2 textureCoord) {
	vec4 diffuseColour = material.diffuseColour;
	if (material.hasDiffuseTexture) {
		// vec4 tex = texture(ue_material.diffuseTexture, textureCoord);
		// if (ue_material.diffuseTextureSRGB)
		// 	tex = pow(tex, vec4(2.2));
		// diffuseColour *= tex;
		diffuseColour *= texture(diffuseTextures[nonuniformEXT(textureIndex)], textureCoord);
	}
	return diffuseColour;
}

vec3 ueGetMaterialSpecular(UEMaterial material, int textureIndex, vec2 textureCoord) {
	vec3 specularColour = vec3(material.specularColour);
	if (material.hasSpecularTexture)
		specularColour *= texture(specularTextures[nonuniformEXT(textureIndex)], textureCoord).rgb;
	return specularColour;
}

float ueGetMaterialShininess(UEMaterial material, int textureIndex, vec2 textureCoord) {
	if (material.hasShininessTexture)
		return texture(shininessTextures[nonuniformEXT(textureIndex)], textureCoord).r;
	else
		return material.shininess;
}

vec3 ueGetMaterialEmissive(UEMaterial material, int textureIndex, vec2 textureCoord) {
	vec3 emissiveColour = vec3(material.emissiveColour);
	if (material.hasEmissiveTexture)
		emissiveColour *= texture(emissiveTextures[nonuniformEXT(textureIndex)], textureCoord).rgb;
	return emissiveColour;
}

vec3 ueGetMaterialNormal(int textureIndex, vec2 textureCoord) {
	vec3 normal = texture(normalMaps[nonuniformEXT(textureIndex)], textureCoord).rgb;
	normal.y = 1 - normal.y;
	normal = normalize(normal * 2.0 - 1.0);
	return normal;
}