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
public:
	/* The constructor */
	BoneAnimationData(unsigned int boneIndex, unsigned int numKeyframePositions, unsigned int numKeyframeRotations, unsigned int numKeyframeScales);

	/* The destructor */
	virtual ~BoneAnimationData() {}

	/* Setters and getters */
	inline void setKeyframePosition(unsigned int index, const Vector3f& position, const float time) { keyframePositions[index] = position; keyframePositionsTimes[index] = time; }
	inline void setKeyframeRotation(unsigned int index, const Quaternion& rotation, const float time) { keyframeRotations[index] = rotation; keyframeRotations[index] = time; }
	inline void setKeyframeScale(unsigned int index, const Vector3f& scale, const float time) { keyframeScales[index] = scale; keyframeScalesTimes[index] = time; }
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
	inline void addChild(unsigned int childIndex) { children.push_back(childIndex); }

	inline const Matrix4f& getFinalTransform() { return finalTransform; }
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
public:
	/* The constructor */
	Skeleton() {}

	/* The destructor */
	virtual ~Skeleton() {}

	/* Setters and getters */
	inline void setGlobalInverseTransform(const Matrix4f& globalInverseTransform) { this->globalInverseTransform = globalInverseTransform; }
	inline void setAnimations(std::vector<Animation*>& animations) { this->animations = animations; }
	inline void setBones(std::vector<Bone*>& bones) { this->bones = bones; }
	inline void setRootBone(unsigned int rootBoneIndex) { this->rootBoneIndex = rootBoneIndex; }

	inline const Matrix4f& getGlobalInverseTransform() { return globalInverseTransform; }
};

#endif /* CORE_RENDER_SKINNING_H_ */
