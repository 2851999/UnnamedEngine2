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

#include "Skinning.h"

#include "../../utils/Logging.h"

/*****************************************************************************
 * The BoneAnimationData class
 *****************************************************************************/

BoneAnimationData::BoneAnimationData(unsigned int boneIndex) {
	this->boneIndex = boneIndex;
}

BoneAnimationData::BoneAnimationData(unsigned int boneIndex, unsigned int numKeyframePositions, unsigned int numKeyframeRotations, unsigned int numKeyframeScales) {
	this->boneIndex = boneIndex;

	keyframePositions.resize(numKeyframePositions);
	keyframePositionsTimes.resize(numKeyframePositions);

	keyframeRotations.resize(numKeyframeRotations);
	keyframeRotationsTimes.resize(numKeyframeRotations);

	keyframeScales.resize(numKeyframeScales);
	keyframeScalesTimes.resize(numKeyframeScales);
}

Matrix4f BoneAnimationData::getTransformMatrix(float animationTime) {
	//Get the interpolated position, rotation and scale at the current time
	Vector3f   position = getInterpolatedPosition(animationTime);
	Quaternion rotation = getInterpolatedRotation(animationTime);
	Vector3f   scale    = getInterpolatedScale   (animationTime);

	//Calculate the matrix and return it
	return Matrix4f().initTranslation(position) * rotation.toRotationMatrix() * Matrix4f().initScale(scale);
}

Vector3f BoneAnimationData::getInterpolatedPosition(float animationTime) {
	//Check whether there is only one keyframe
	if (keyframePositions.size() == 1)
		return keyframePositions[0];
	//Get the index of the last keyframe
	unsigned int lastIndex = getPositionIndex(animationTime);
	//Calculate the index of the next keyframe
	unsigned int nextIndex = lastIndex + 1;

	//Calculate the difference in the times between the two keyframes
	float deltaTime = keyframePositionsTimes[nextIndex] - keyframePositionsTimes[lastIndex];
	//Calculate the interpolation factor
	float factor = (animationTime - keyframePositionsTimes[lastIndex]) / deltaTime;
	//Interpolate and return the result
	return Vector3f::lerp(keyframePositions[lastIndex], keyframePositions[nextIndex], factor);
}

Quaternion BoneAnimationData::getInterpolatedRotation(float animationTime) {
	//Check whether there is only one keyframe
	if (keyframeRotations.size() == 1)
		return keyframeRotations[0];
	//Get the index of the last keyframe
	unsigned int lastIndex = getRotationIndex(animationTime);
	//Calculate the index of the next keyframe
	unsigned int nextIndex = lastIndex + 1;

	//Calculate the difference in the times between the two keyframes
	float deltaTime = keyframeRotationsTimes[nextIndex] - keyframeRotationsTimes[lastIndex];
	//Calculate the interpolation factor
	float factor = (animationTime - keyframeRotationsTimes[lastIndex]) / deltaTime;
	//Interpolate and return the result
	return Quaternion::slerp(keyframeRotations[lastIndex], keyframeRotations[nextIndex], factor).normalise();
}

Vector3f BoneAnimationData::getInterpolatedScale(float animationTime) {
	//Check whether there is only one keyframe
	if (keyframeScales.size() == 1)
		return keyframeScales[0];
	//Get the index of the last keyframe
	unsigned int lastIndex = getScaleIndex(animationTime);
	//Calculate the index of the next keyframe
	unsigned int nextIndex = lastIndex + 1;

	//Calculate the difference in the times between the two keyframes
	float deltaTime = keyframeScalesTimes[nextIndex] - keyframeScalesTimes[lastIndex];
	//Calculate the interpolation factor
	float factor = (animationTime - keyframeScalesTimes[lastIndex]) / deltaTime;
	//Interpolate and return the result
	return Vector3f::lerp(keyframeScales[lastIndex], keyframeScales[nextIndex], factor);
}

unsigned int BoneAnimationData::getPositionIndex(float animationTime) {
	for (unsigned int i = lastPositionsIndex; i < keyframePositionsTimes.size() - 1; i++) {
		if (animationTime < keyframePositionsTimes[i + 1] && animationTime >= keyframePositionsTimes[i]) {
			lastPositionsIndex = i;
			return i;
		}
	}
	for (unsigned int i = 0; i <= lastPositionsIndex; i++) {
		if (animationTime < keyframePositionsTimes[i + 1]) {
			lastPositionsIndex = i;
			return i;
		}
	}
	//Log an error
	Logger::log("Position not found for animation at a time of '" + utils_string::str(animationTime) + "'", "BoneAnimationData", LogType::Error);
	return 0;
}

unsigned int BoneAnimationData::getRotationIndex(float animationTime) {
	for (unsigned int i = lastRotationsIndex; i < keyframeRotationsTimes.size() - 1; i++) {
		if (animationTime < keyframeRotationsTimes[i + 1] && animationTime >= keyframeRotationsTimes[i]) {
			lastRotationsIndex = i;
			return i;
		}
	}
	for (unsigned int i = 0; i <= lastRotationsIndex; i++) {
		if (animationTime < keyframeRotationsTimes[i + 1]) {
			lastRotationsIndex = i;
			return i;
		}
	}
	//Log an error
	Logger::log("Rotation not found for animation at a time of '" + utils_string::str(animationTime) + "'", "BoneAnimationData", LogType::Error);
	return 0;
}

unsigned int BoneAnimationData::getScaleIndex(float animationTime) {
	for (unsigned int i = lastScalesIndex; i < keyframeScalesTimes.size() - 1; i++) {
		if (animationTime < keyframeScalesTimes[i + 1] && animationTime >= keyframeScalesTimes[i]) {
			lastScalesIndex = i;
			return i;
		}
	}
	for (unsigned int i = 0; i <= lastScalesIndex; i++) {
		if (animationTime < keyframeScalesTimes[i + 1]) {
			lastScalesIndex = i;
			return i;
		}
	}
	//Log an error
	Logger::log("Scale not found for animation at a time of '" + utils_string::str(animationTime) + "'", "BoneAnimationData", LogType::Error);
	return 0;
}

/*****************************************************************************
 * The Bone class
 *****************************************************************************/

Bone::Bone(std::string name, Matrix4f transform) : name(name), transform(transform) {
	offset.setIdentity();
	finalTransform = Matrix4f().initIdentity();
}

/*****************************************************************************
 * The Animation class
 *****************************************************************************/

Animation::Animation(std::string name, float ticksPerSecond, float duration) : name(name), ticksPerSecond(ticksPerSecond), duration(duration) {

}

BoneAnimationData* Animation::getBoneAnimationData(unsigned int boneIndex) {
	//Go through the animations
	for (unsigned int i = 0; i < boneData.size(); i++) {
		//Check the current animation's name
		if (boneData[i]->getBoneIndex() == boneIndex)
			//Return the animation
			return boneData[i];
	}
	//Log an error
	//Logger::log("BoneAnimationData with the index '" + utils_string::str(boneIndex) + "' was not found in the animation with the name '" + name + "'", "Animation", LogType::Error);
	//Return NULL if not found
	return NULL;
}

/*****************************************************************************
 * The Skeleton class
 *****************************************************************************/

void Skeleton::updateBone(float animationTime, Bone* parentBone, const Matrix4f& parentMatrix) {
	//Calculate the matrix for the current bone
	Matrix4f nodeTransformation;
	if (parentBone->getAnimationData())
		nodeTransformation = parentBone->getAnimationData()->getTransformMatrix(animationTime);
	else
		nodeTransformation = parentBone->getTransform();
	Matrix4f globalTransformation = parentMatrix * nodeTransformation;
	//Calculate the final transformation for the current bone
	Matrix4f finalTransformation = globalInverseTransform * globalTransformation * parentBone->getOffset();
	//Assign the bone's final transformation
	parentBone->setFinalTransform(finalTransformation);

//	std::cout << "TRANSFORM" << std::endl;
//	std::cout << parentBone->getTransform().toString() << std::endl;
//	std::cout << "OFFSET" << std::endl;
//	std::cout << parentBone->getOffset().toString() << std::endl;
//	std::cout << "GLOBAL INVERSE TRANSFORMATION" << std::endl;
//	std::cout << globalInverseTransform.toString() << std::endl;
//	std::cout << "GLOBAL TRANSFORMATION" << std::endl;
//	std::cout << globalTransformation.toString() << std::endl;
//	std::cout << "FINAL TRANSFORMATION" << std::endl;
//	std::cout << parentBone->getFinalTransform().toString() << std::endl;
//	std::cout << "END" << std::endl;
	//Go through each child bone
	for (unsigned int i = 0; i < parentBone->getNumChildren(); i++)
		//Update the current bone
		updateBone(animationTime, bones[parentBone->getChild(i)], globalTransformation);
}

void Skeleton::setBoneBindPose(Bone* parentBone, const Matrix4f& parentMatrix) {
	//Calculate the matrix for the current bone
	Matrix4f nodeTransformation = parentBone->getTransform();
	Matrix4f globalTransformation = parentMatrix * nodeTransformation;
	//Calculate the final transformation for the current bone
	Matrix4f finalTransformation = globalInverseTransform * globalTransformation * parentBone->getOffset();
	//Assign the bone's final transformation
	parentBone->setFinalTransform(finalTransformation);
	//Go through each child bone
	for (unsigned int i = 0; i < parentBone->getNumChildren(); i++)
		//Update the current bone
		setBoneBindPose(bones[parentBone->getChild(i)], globalTransformation);
}

void Skeleton::updateBones(float animationTime) {
	//Identity matrix
	Matrix4f identity = Matrix4f().initIdentity();

	//Update the root bone
	updateBone(animationTime, bones[rootBoneIndex], identity);
}

void Skeleton::update(float deltaSeconds) {
	//Check whether an animation is occurring
	if (currentAnimation) {
		//Add to the current animation time
		currentTime += deltaSeconds;

		//Get the current animation time
		float timeInTicks = currentTime * currentAnimation->getTicksPerSecond();
		float animationTime = fmod(timeInTicks, currentAnimation->getDuration());

		//Update the bones
		updateBones(animationTime);
	}
}

void Skeleton::startAnimation(std::string name) {
	//Assign the current animation
	currentAnimation = getAnimation(name);
	//Reset the time
	currentTime = 0;
	//Ensure the animation was found
	if (currentAnimation) {
		//Go through each bone and assign their animation data
		for (unsigned int i = 0; i < bones.size(); i++)
			bones[i]->setAnimationData(currentAnimation->getBoneAnimationData(i));
	}
}

void Skeleton::stopAnimation() {
	setBindPose();
	currentAnimation = NULL;
}

void Skeleton::setBindPose() {
	//Identity matrix
	Matrix4f identity = Matrix4f().initIdentity();

	//Update the root bone
	setBoneBindPose(bones[rootBoneIndex], identity);
}

//void Skeleton::print(std::string prefix, Bone* current) {
//	prefix += current->getName();
//	std::cout << prefix << std::endl;
//	for (unsigned int i = 0; i < current->getNumChildren(); i++) {
//		print(prefix + "/", bones[current->getChild(i)]);
//	}
//}

Animation* Skeleton::getAnimation(std::string name) {
	//Go through the animations
	for (unsigned int i = 0; i < animations.size(); i++) {
		//Check the current animation's name
		if (animations[i]->getName() == name)
			//Return the animation
			return animations[i];
	}
	//Log an error
	Logger::log("Animation with the name '" + name + "' was not found in the skeleton", "Skeleton", LogType::Error);
	//Return NULL if not found
	return NULL;
}
