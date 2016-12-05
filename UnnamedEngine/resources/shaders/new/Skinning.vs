#map attribute BoneIDs boneIDs
#map attribute BoneWeights boneWeights
#map uniform UseSkinning useSkinning

#define UE_LOCATION_BONE_IDS 2
#define UE_LOCATION_BONE_WEIGHTS 3

const int MAX_BONES = 80;
uniform mat4 bones[MAX_BONES];

layout(location = UE_LOCATION_BONE_IDS) in ivec4 boneIDs;
layout(location = UE_LOCATION_BONE_WEIGHTS) in vec4  boneWeights;

uniform bool useSkinning;

mat4 ue_getBoneTransform() {
	mat4 boneTransform = bones[boneIDs[0]] * boneWeights[0];
	boneTransform += bones[boneIDs[1]] * boneWeights[1];
	boneTransform += bones[boneIDs[2]] * boneWeights[2];
	boneTransform += bones[boneIDs[3]] * boneWeights[3];
	
	return boneTransform;
}
