#map attribute BoneIDs ue_boneIDs
#map attribute BoneWeights ue_boneWeights
#map uniform UseSkinning ue_useSkinning

const int UE_MAX_BONES = 80;
uniform mat4 ue_bones[UE_MAX_BONES];

layout(location = UE_LOCATION_BONE_IDS) in ivec4 ue_boneIDs;
layout(location = UE_LOCATION_BONE_WEIGHTS) in vec4  ue_boneWeights;

uniform bool ue_useSkinning;

mat4 ueGetBoneTransform() {
	mat4 boneTransform = ue_bones[ue_boneIDs[0]] * ue_boneWeights[0];
	boneTransform += ue_bones[ue_boneIDs[1]] * ue_boneWeights[1];
	boneTransform += ue_bones[ue_boneIDs[2]] * ue_boneWeights[2];
	boneTransform += ue_bones[ue_boneIDs[3]] * ue_boneWeights[3];
	
	return boneTransform;
}
