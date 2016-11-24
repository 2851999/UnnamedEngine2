/*****************************************************************************
 *
 *   Copyright 2016 Joel Davies
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *****************************************************************************/

#ifndef CORE_RENDER_SKINNING_H_
#define CORE_RENDER_SKINNING_H_

#include <vector>

#include "../Vector.h"
#include "../Matrix.h"

/*****************************************************************************
 * The BoneAnimationData class contains information about an animation for
 * a particular bone
 *****************************************************************************/

class BoneAnimationData {
private:
	/* The index of the bone this animation data is for in the skeleton
	 * instance */
	unsigned int boneIndex;

	/* The key-frame positions and times */
	std::vector<Vector3f> keyframePositions;
	std::vector<float>    keyframePositionsTimes;

	/* The key-frame rotations and times */
	std::vector<Quaternion> keyframeRotations;
	std::vector<float>      keyframeRotationsTimes;

	/* The key-frame scales and times */
	std::vector<Vector3f> keyframeScales;
	std::vector<float>    keyframeScalesTimes;

	/* The indices of the last locations checked */
	unsigned int lastPositionsIndex = 0;
	unsigned int lastRotationsIndex = 0;
	unsigned int lastScalesIndex    = 0;
public:
	/* The constructor */
	BoneAnimationData(unsigned int boneIndex, unsigned int numKeyframePositions, unsigned int numKeyframeRotations, unsigned int numKeyframeScales);

	/* The destructor */
	virtual ~BoneAnimationData() {}

	/* Method used to calculate and return the transformation matrix at a given time */
	Matrix4f getTransformMatrix(float animationTime);

	/* Methods used to calculate and return the interpolated position/rotation/scale at a given time */
	Vector3f   getInterpolatedPosition(float animationTime);
	Quaternion getInterpolatedRotation(float animationTime);
	Vector3f   getInterpolatedScale   (float animationTime);

	/* Method used to find and return the index of the last position/rotation/scale keyframe for a given time */
	unsigned int getPositionIndex(float animationTime);
	unsigned int getRotationIndex(float animationTime);
	unsigned int getScaleIndex   (float animationTime);

	/* Setters and getters */
	inline void setKeyframePosition(unsigned int index, const Vector3f& position, const float time) { keyframePositions[index] = position; keyframePositionsTimes[index] = time; }
	inline void setKeyframeRotation(unsigned int index, const Quaternion& rotation, const float time) { keyframeRotations[index] = rotation; keyframeRotationsTimes[index] = time; }
	inline void setKeyframeScale(unsigned int index, const Vector3f& scale, const float time) { keyframeScales[index] = scale; keyframeScalesTimes[index] = time; }

	inline unsigned int getBoneIndex() { return boneIndex; }
};

/*****************************************************************************
 * The Bone class forms part of a Skeleton
 *****************************************************************************/

class Bone {
private:
	/* The name of this bone */
	std::string name;

	/* The transformation matrix for this bone (without any animation) */
	Matrix4f transform;

	/* The offset for this bone */
	Matrix4f offset;

	/* The final transformation of this bone (Computed for animations) */
	Matrix4f finalTransform;

	/* The indices for the child bones of this bone */
	std::vector<unsigned int> children;

	/* Pointer to the current BoneAnimationData instance for the current
	 * animation being executed */
	BoneAnimationData* animationData = NULL;
public:
	/* The constructor */
	Bone(std::string name, Matrix4f transform);

	/* The destructor */
	virtual ~Bone() {}

	/* Setters and getters */
	inline void setTransform(const Matrix4f& transform) { this->transform = transform; }
	inline void setOffset(const Matrix4f& offset) { this->offset = offset; }
	inline void setFinalTransform(const Matrix4f& finalTransform) { this->finalTransform = finalTransform; }
	inline void addChild(unsigned int childIndex) { children.push_back(childIndex); }
	inline void setAnimationData(BoneAnimationData* animationData) { this->animationData = animationData; }

	inline Matrix4f& getTransform() { return transform; }
	inline Matrix4f& getOffset() { return offset; }
	inline Matrix4f& getFinalTransform() { return finalTransform; }
	inline unsigned int getNumChildren() { return children.size(); }
	inline unsigned int getChild(unsigned int index) { return children[index]; }
	inline BoneAnimationData* getAnimationData() { return animationData; }
};

/*****************************************************************************
 * The Animation class contains the information about a particular animation
 *****************************************************************************/

class Animation {
private:
	/* The name of this animation */
	std::string name;

	/* The number of ticks executed per second */
	float ticksPerSecond;

	/* The duration of this animation */
	float duration;

	/* The bone animation data for this animation */
	std::vector<BoneAnimationData*> boneData;
public:
	/* The constructor */
	Animation(std::string name, float ticksPerSecond, float duration);

	/* The destructor */
	virtual ~Animation() {}

	/* Setters and getters */
	inline void setName(std::string name) { this->name = name; }
	inline void setTicksPerSecond(float ticksPerSecond) { this->ticksPerSecond = ticksPerSecond; }
	inline void setDuration(float duration) { this->duration = duration; }
	inline void setBoneData(std::vector<BoneAnimationData*>& boneData) { this->boneData = boneData; }

	inline std::string getName() { return name; }
	inline float getTicksPerSecond() { return ticksPerSecond; }
	inline float getDuration() { return duration; }
	BoneAnimationData* getBoneAnimationData(unsigned int boneIndex);
};

/*****************************************************************************
 * The Skeleton class contains the information about a skeleton
 *****************************************************************************/

class Skeleton {
private:
	/* The global inverse transform instance */
	Matrix4f globalInverseTransform;

	/* The animations within this skeleton */
	std::vector<Animation*> animations;

	/* The current animation being executed */
	Animation* currentAnimation = NULL;

	/* The bones in this skeleton */
	std::vector<Bone*> bones;

	/* The index of the root bone */
	unsigned int rootBoneIndex = 0;

	/* The current time of an animation */
	float currentTime = 0;

	/* Recursive method used to update a bone */
	void updateBone(float animationTime, Bone* parentBone, Matrix4f& parentMatrix);

	/* Recursive method used to set the bind pose of a bone */
	void setBoneBindPose(Bone* parentBone, Matrix4f& parentMatrix);
public:
	/* The constructor */
	Skeleton() {}

	/* The destructor */
	virtual ~Skeleton() {}

	/* Method called to update all of the bones given an animation time */
	void updateBones(float animationTime);

	/* Method called to update this model */
	void update(float deltaSeconds);

	/* Method called to start playing an animation */
	void startAnimation(std::string name);

	/* Method called to stop playing an animation */
	void stopAnimation();

	/* Method called to setup the bind pose of this skeleton */
	void setBindPose();

	/* Setters and getters */
	inline void setGlobalInverseTransform(const Matrix4f& globalInverseTransform) { this->globalInverseTransform = globalInverseTransform; }
	inline void setAnimations(std::vector<Animation*>& animations) { this->animations = animations; }
	inline void setBones(std::vector<Bone*>& bones) { this->bones = bones; }
	inline void setRootBone(unsigned int rootBoneIndex) { this->rootBoneIndex = rootBoneIndex; }

	inline const Matrix4f& getGlobalInverseTransform() { return globalInverseTransform; }
	Animation* getAnimation(std::string name);
	inline unsigned int getNumBones() { return bones.size(); }
	inline Bone* getBone(unsigned int index) { return bones[index]; }
};

#endif /* CORE_RENDER_SKINNING_H_ */
