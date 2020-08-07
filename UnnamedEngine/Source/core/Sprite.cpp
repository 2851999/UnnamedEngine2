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
	timer.reset();
}

Animation2D::~Animation2D() {
	
}

void Animation2D::start() {
	//Start the animation, after ensuring all the variables are reset ready to start
	reset();
	timer.start();
	onStart();
}

void Animation2D::pause() {
	//Pause the animation
	timer.pause();
}

void Animation2D::resume() {
	//Resume the animation
	timer.resume();
}

void Animation2D::stop() {
	//Stop
	timer.stop();
	onStop();
}

void Animation2D::reset() {
	//Reset all of the values
	currentFrame = startFrame;
	timer.reset();
	onReset();
}

void Animation2D::update() {
	//Check whether the animation is running
	if (isRunning()) {
		//Obtain the current frame that should be visible
		unsigned int currentFrameIndex = startFrame + (unsigned int) std::floor(timer.getSeconds() / timeBetweenFrames);
		//Check whether the animation should update
		if (currentFrameIndex != currentFrame) {
			//Check whether the current frame is the last (NOTE: First frame can be index 0)
			if (currentFrame >= startFrame + totalFrames - 1) {
				//Restart the animation if it should repeat
				if (repeat) {
					currentFrame = startFrame;
					timer.restart();
					//Update the animation to the current frame
					updateFrame();
				} else
					stop();
			} else {
				//Assign the frame
				currentFrame = currentFrameIndex;
				//Update the animation to the current frame
				updateFrame();
			}
		}
	}
}

void Animation2D::updateTimeBetweenFrames(float time) {
	//Check if the animation is running
	if (isRunning() && time != timeBetweenFrames) {
		//Calculate the new frame in the animation
		unsigned int currentFrameIndex = (unsigned int) std::floor(timer.getSeconds() / timeBetweenFrames);

		float newFrameTime = (currentFrameIndex + ((timer.getSeconds() / timeBetweenFrames) - ((float) currentFrameIndex))) * time;

		//Assign the new time
		timer.setSeconds(newFrameTime);
	}
	//Assign the time
	this->timeBetweenFrames = time;
}

/*****************************************************************************
 * The TextureAnimation2D class
 *****************************************************************************/

TextureAnimation2D::TextureAnimation2D(Sprite* sprite, TextureAtlas* textureAtlas, float timeBetweenFrame, bool repeat, unsigned int startFrame, int numFrames) :
		Animation2D(sprite, timeBetweenFrame, numFrames == -1 ? textureAtlas->getNumTextures() : numFrames, repeat, startFrame), textureAtlas(textureAtlas) {
	textureLayers.push_back(textureAtlas->getTexture());
}

void TextureAnimation2D::addMaxLayers(unsigned int maxLayers) {
	while (textureLayers.size() < maxLayers)
		textureLayers.push_back(NULL);
}

void TextureAnimation2D::onStart() {
	//Assign the texture in the entity
	if (sprite) {
		//Check the number of layers required have been allocated
		if (sprite->getNumLayers() < textureLayers.size())
			Logger::log("Not enough Sprite layers assigned for animation", "TextureAnimation2D", LogType::Error);
		unsigned int numVisible = 0;
		//Assign the required textures
		for (unsigned int i = 0; i < textureLayers.size(); ++i) {
			sprite->setLayer(numVisible, textureLayers[i]);
			if (textureLayers[i])
				++numVisible;
		}
		//Assign the visible layers
		sprite->setVisibleLayers(numVisible);
		//Assign the texture for the first frame
		sprite->setTextureCoords(textureAtlas, currentFrame);
	}
}

void TextureAnimation2D::updateFrame() {
	//Ensure the entity has been assigned
	if (sprite)
		//Assign the texture coordinates
		sprite->setTextureCoords(textureAtlas, currentFrame);
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

void Sprite::setupMesh(Texture* texture) {
	setMesh(new Mesh(MeshBuilder::createQuad(getWidth(), getHeight(), texture, MeshData::SEPARATE_TEXTURE_COORDS)), Renderer::getRenderShader(Renderer::SHADER_MATERIAL), DataUsage::DYNAMIC);
	//Add a sub data
	getMesh()->getData()->addSubData(0, 0, 6, 0);
	getMaterial()->setDiffuse(texture);
	getMaterial()->update();
}

void Sprite::setup(Texture* texture) {
	setWidth(texture->getWidth());
	setHeight(texture->getHeight());
	setupMesh(texture);
}

void Sprite::setup(Texture* texture, float width, float height) {
	setWidth(width);
	setHeight(height);
	setupMesh(texture);
}

void Sprite::setup(TextureAtlas* textureAtlas) {
	setWidth(textureAtlas->getSubTextureWidth());
	setHeight(textureAtlas->getSubTextureHeight());
	setupMesh(textureAtlas->getTexture());
	setVisibleLayers(getNumLayers());
}

void Sprite::setup(TextureAtlas* textureAtlas, float width, float height) {
	setWidth(width);
	setHeight(height);
	setupMesh(textureAtlas->getTexture());
	setVisibleLayers(getNumLayers());
}

void Sprite::addMaxLayers(unsigned int maxLayers) {
	while (getMesh()->getNumMaterials() < maxLayers) {
		//Add a Material for this new layer
		Material* material = new Material();
		material->setup();
		getMesh()->addMaterial(material);
	}
}

void Sprite::setLayer(unsigned int layer, Texture* texture) {
	//Assign the required texture
	getMesh()->getMaterial(layer)->setDiffuse(texture);
	getMesh()->getMaterial(layer)->update();
}

void Sprite::setVisibleLayers(unsigned int count) {
	unsigned int endIndex = count - 1;
	//Get the current maximum index
	unsigned int currentMaxIndex = getMesh()->getData()->getSubDataCount() - 1;
	//Check whether some should be added or removed
	if (currentMaxIndex > endIndex) {
		//Remove the last few
		for (unsigned int i = endIndex; i < currentMaxIndex; ++i)
			getMesh()->getData()->removeSubData(endIndex + 1);
	} else if (currentMaxIndex < endIndex) {
		//Add the required ones
		for (unsigned int i = currentMaxIndex; i < endIndex; ++i)
			getMesh()->getData()->addSubData(0, 0, 6, i + 1);
	}
}

void Sprite::update() {
	//Check whether there is a current animation
	if (currentAnimation) {
		//Update the current animation
		currentAnimation->update();
		//Check if the animation has finished
		if (currentAnimation->isStopped())
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

