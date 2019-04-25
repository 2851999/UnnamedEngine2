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

#include "render/Renderer.h"
#include "../utils/Logging.h"

/*****************************************************************************
 * The Animation2D class
 *****************************************************************************/

Animation2D::Animation2D(Sprite* sprite, float timeBetweenFrame, unsigned int totalFrames, bool repeat, unsigned int startFrame) :
		sprite(sprite), timeBetweenFrames(timeBetweenFrame), totalFrames(totalFrames), repeat(repeat), startFrame(startFrame) {
	currentFrame = startFrame;
	running = false;
	currentTime = 0.0f;
}

Animation2D::~Animation2D() {

}

void Animation2D::start() {
	//Start the animation, after ensuring all the variables are reset ready to start
	reset();
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
	currentFrame = startFrame;
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
		if (currentTime >= timeBetweenFrames) {
			//Check whether the current frame is the last (NOTE: First frame can be index 0)
			if (currentFrame == startFrame + totalFrames - 1) {
				//Restart the animation if it should repeat
				if (repeat) {
					currentFrame = startFrame;
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

TextureAnimation2D::TextureAnimation2D(Sprite* sprite, TextureAtlas* textureAtlas, float timeBetweenFrame, bool repeat, unsigned int startFrame, int numFrames) :
		Animation2D(sprite, timeBetweenFrame, numFrames == -1 ? textureAtlas->getNumTextures() : numFrames, repeat, startFrame), textureAtlas(textureAtlas) {

}

void TextureAnimation2D::onStart() {
	//Assign the texture in the entity
	if (sprite) {
		//Assign the texture
		sprite->getMaterial()->diffuseTexture = textureAtlas->getTexture();
		//Assign the texture for the first frame
		sprite->setTextureCoords(textureAtlas, currentFrame);
	}
}

void TextureAnimation2D::updateFrame() {
	//Ensure the entity has been assigned
	if (sprite) {
		//Assign the texture coordinates
		sprite->setTextureCoords(textureAtlas, currentFrame);
	}
}

/*****************************************************************************
 * The Sprite class
 *****************************************************************************/

Sprite::~Sprite() {
	//Go through and release all of the animations
	for (auto const& elem : animations)
		delete elem.second;
	animations.clear();
}

void Sprite::setup(Texture* texture) {
	setWidth(texture->getWidth());
	setHeight(texture->getHeight());
	setMesh(new Mesh(MeshBuilder::createQuad(getWidth(), getHeight(), texture, MeshData::SEPARATE_TEXTURE_COORDS)), Renderer::getRenderShader(Renderer::SHADER_MATERIAL));
	getMaterial()->diffuseTexture = texture;
}

void Sprite::setup(Texture* texture, float width, float height) {
	setWidth(width);
	setHeight(height);
	setMesh(new Mesh(MeshBuilder::createQuad(getWidth(), getHeight(), texture, MeshData::SEPARATE_TEXTURE_COORDS)), Renderer::getRenderShader(Renderer::SHADER_MATERIAL));
	getMaterial()->diffuseTexture = texture;
}

void Sprite::setup(TextureAtlas* textureAtlas) {
	setWidth(textureAtlas->getSubTextureWidth());
	setHeight(textureAtlas->getSubTextureHeight());
	setMesh(new Mesh(MeshBuilder::createQuad(getWidth(), getHeight(), textureAtlas->getTexture(), MeshData::SEPARATE_TEXTURE_COORDS)), Renderer::getRenderShader(Renderer::SHADER_MATERIAL));
	getMaterial()->diffuseTexture = textureAtlas->getTexture();
}

void Sprite::setup(TextureAtlas* textureAtlas, float width, float height) {
	setWidth(width);
	setHeight(height);
	setMesh(new Mesh(MeshBuilder::createQuad(getWidth(), getHeight(), textureAtlas->getTexture(), MeshData::SEPARATE_TEXTURE_COORDS)), Renderer::getRenderShader(Renderer::SHADER_MATERIAL));
	getMaterial()->diffuseTexture = textureAtlas->getTexture();
}

void Sprite::update(float deltaSeconds) {
	//Check whether there is a current animation
	if (currentAnimation) {
		//Update the current animation
		currentAnimation->update(deltaSeconds);
		//Check if the animation has finished
		if (! currentAnimation->isRunning())
			stopAnimation();
	}
	//Update this object
	GameObject2D::update();
}

void Sprite::startAnimation(std::string name) {
	//Check whether the animation exists
	if (animations.count(name) > 0) {
		//Assign the current animation
		currentAnimationName = name;
		currentAnimation = animations.at(name);
		//Start the current animation
		currentAnimation->start();
	} else
		//Log an error
		Logger::log("Could not locate the animation with the name '" + name + "'", "Sprite", LogType::Error);
}

void Sprite::stopAnimation() {
	//Ensure there is a current animation
	if (currentAnimation) {
		//Stop the current animation
		currentAnimation->stop();
		currentAnimationName = "";
		currentAnimation = NULL;
	}
}

void Sprite::setTextureCoords(TextureAtlas* textureAtlas, unsigned int index) {
	//Ensure the entity has been assigned with a mesh
	if (hasMesh()) {
		//Update the mesh
		float top, left, bottom, right;
		textureAtlas->getSides(index, top, left, bottom, right);
		getMesh()->getData()->clearTextureCoords();
		MeshBuilder::addQuadT(getMesh()->getData(), top, left, bottom, right);
		getMesh()->getRenderData()->updateTextureCoords();
	}
}

