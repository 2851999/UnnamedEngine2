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

#include "Sprite.h"

#include "../utils/Logging.h"

/*****************************************************************************
 * The Animation2D class
 *****************************************************************************/

Animation2D::Animation2D(GameObject2D* entity, float timeBetweenFrame, unsigned int totalFrames, bool repeat) :
		entity(entity), timeBetweenFrame(timeBetweenFrame), totalFrames(totalFrames), repeat(repeat) {
	currentFrame = 0;
	running = false;
	currentTime = 0.0f;
}

Animation2D::~Animation2D() {

}

void Animation2D::start() {
	//Start
	running = true;
	onStart();
}

void Animation2D::stop() {
	//Stop
	running = false;
	onStop();
}

void Animation2D::reset() {
	//Reset all of the values
	currentFrame = 0;
	running = false;
	currentTime = 0.0f;
	onReset();
}

void Animation2D::update(float deltaSeconds) {
	//Check whether the animation is running
	if (running) {
		//Increment the current time
		currentTime += deltaSeconds;

		//Check whether the animation should update
		if (currentTime >= timeBetweenFrame) {
			//Check whether the current frame is the last (NOTE: First frame is index 0)
			if (currentFrame == totalFrames - 1) {
				//Restart the animation if it should repeat
				if (repeat) {
					currentFrame = 0;
					currentTime = 0;
					//Update the animation to the current frame
					updateFrame();
				} else
					stop();
			} else {
				//Increment the current frame, and reset the time
				currentFrame++;
				currentTime = 0;
				//Update the animation to the current frame
				updateFrame();
			}
		}
	}
}

/*****************************************************************************
 * The TextureAnimation2D class
 *****************************************************************************/

TextureAnimation2D::TextureAnimation2D(GameObject2D* entity, TextureAtlas* textureAtlas, float timeBetweenFrame, bool repeat) :
		Animation2D(entity, timeBetweenFrame, textureAtlas->getNumTextures(), repeat), textureAtlas(textureAtlas) {

}

void TextureAnimation2D::onStart() {
	//Assign the texture in the entity
	if (entity && entity->hasMaterial())
		//Assign the texture
		entity->getMaterial()->setDiffuseTexture(textureAtlas->getTexture());
}

void TextureAnimation2D::updateFrame() {
	//Ensure the entity has been assigned with a mesh
	if (entity && entity->hasMesh()) {
		//Update the mesh
		float top, left, bottom, right;
		textureAtlas->getSides(currentFrame, top, left, bottom, right);
		entity->getMesh()->getData()->clearTextureCoords();
		MeshBuilder::addQuadT(entity->getMesh()->getData(), top, left, bottom, right);
		entity->getMesh()->getRenderData()->updateTextureCoords();
	}
}

/*****************************************************************************
 * The Sprite2D class
 *****************************************************************************/

Sprite2D::Sprite2D(Texture* texture) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(texture->getWidth(), texture->getHeight(), texture, MeshData::SEPARATE_TEXTURE_COORDS)), "Material", texture->getWidth(), texture->getHeight()) {
	getMaterial()->setDiffuseTexture(texture);
}

Sprite2D::Sprite2D(Texture* texture, float width, float height) :
		GameObject2D(new Mesh(MeshBuilder::createQuad(width, height, texture, MeshData::SEPARATE_TEXTURE_COORDS)), "Material", width, height) {
	getMaterial()->setDiffuseTexture(texture);
}

Sprite2D::~Sprite2D() {
	//Go through and release all of the animations
	for (auto const& elem : animations)
		delete elem.second;
	animations.clear();
}

void Sprite2D::update(float deltaSeconds) {
	//Check whether there is a current animation
	if (currentAnimation)
		//Update the current animation
		currentAnimation->update(deltaSeconds);
	//Update this object
	GameObject2D::update();
}

void Sprite2D::startAnimation(std::string name) {
	//Check whether the animation exists
	if (animations.count(name) > 0) {
		//Assign the current animation
		currentAnimation = animations.at(name);
		//Start the current animation
		currentAnimation->start();
	} else
		//Log an error
		Logger::log("Could not locate the animation with the name '" + name + "'", "Sprite2D", LogType::Error);
}

void Sprite2D::stopAnimation() {
	//Ensure there is a current animationn
	if (currentAnimation) {
		//Stop the current animation
		currentAnimation->stop();
		currentAnimation = NULL;
	}
}

