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

#ifndef CORE_SPRITE_H_
#define CORE_SPRITE_H_

#include "Object.h"
#include "render/TextureAtlas.h"

#include "../utils/Timer.h"

/* Forward declaration of Sprite */
class Sprite;

/*****************************************************************************
 * The Animation2D class helps to create 2D animations
 *****************************************************************************/

class Animation2D {
protected:
	/* The entity to apply this animation to */
	Sprite* sprite = NULL;

	/* The current frame of the animation */
	unsigned int currentFrame = 0;
private:
	/* The time between each frame of the animation (in seconds) */
	float timeBetweenFrame = 0.0f;

	/* The total number of frames within this animation */
	unsigned int totalFrames = 0;

	/* States whether this animation is running */
	bool running = false;

	/* States whether this animation should keep repeating */
	bool repeat = false;

	/* The start frame in this animation */
	unsigned int startFrame;

	/* The current time for the animation */
	float currentTime = 0.0f;
public:
	/* The constructors */
	Animation2D(Sprite* sprite, float timeBetweenFrame, unsigned int totalFrames, bool repeat = false, unsigned int startFrame = 0);
	Animation2D(float timeBetweenFrame, unsigned int totalFrames, bool repeat = false, unsigned int startFrame = 0) : Animation2D(NULL, timeBetweenFrame, totalFrames, repeat, startFrame) {}

	/* The destructor */
	virtual ~Animation2D();

	/* Methods called to start/stop/reset/restart this animation */
	void start(); //This will continue where it left of, unless reset() is called
	void stop();
	void reset();
	inline void restart() { reset(); start(); }

	/* Method called to update this animation */
	void update(float deltaSeconds);

	/* Various methods that are called when certain things happen */
	virtual void onStart() {}
	virtual void onStop() {}
	virtual void onReset() {}

	/* Method called when the frame needs to change */
	virtual void updateFrame() {}

	/* Setters and getters */
	inline void setTimeBetweenFrame(float time) { timeBetweenFrame = time; }
	inline void setRepeat(bool repeat) { this->repeat = repeat; }
	inline void setSprite(Sprite* sprite) { this->sprite = sprite; }

	inline float getTimeBetweenFrame() { return timeBetweenFrame; }
	inline bool  doesRepeat() { return repeat; }
	inline bool  isRunning()  { return running; }
	inline Sprite* getSprite() { return sprite; }
};

/*****************************************************************************
 * The TextureAnimation2D class helps to create 2D texture animations
 *****************************************************************************/

class TextureAnimation2D : public Animation2D {
private:
	/* The texture atlas for the animation */
	TextureAtlas* textureAtlas;
public:
	/* The constructors */
	TextureAnimation2D(Sprite* sprite, TextureAtlas* textureAtlas, float timeBetweenFrame, bool repeat = false, unsigned int startFrame = 0, int numFrames = -1);
	TextureAnimation2D(TextureAtlas* textureAtlas, float timeBetweenFrame, bool repeat = false, unsigned int startFrame = 0, int numFrames = -1) : TextureAnimation2D(NULL, textureAtlas, timeBetweenFrame, repeat, startFrame, numFrames) {}

	/* Method called when the frame needs to change */
	virtual void updateFrame() override;

	/* Called when this animation starts */
	virtual void onStart();
};

/*****************************************************************************
 * The Sprite class helps to create a 2D sprite that can have animations
 * attached to them
 *****************************************************************************/

class Sprite : public GameObject2D {
private:
	/* The animations for this sprite */
	std::unordered_map<std::string, Animation2D*> animations;

	/* The current animation being played */
	std::string currentAnimationName = "";
	Animation2D* currentAnimation = NULL;
public:
	/* The constructors */
	Sprite() {}
	Sprite(Texture* texture) { setup(texture); }
	Sprite(Texture* texture, float width, float height) { setup(texture, width, height); }
	Sprite(TextureAtlas* textureAtlas) { setup(textureAtlas); }
	Sprite(TextureAtlas* textureAtlas, float width, float height) { setup(textureAtlas, width, height); }

	/* Sets the mesh given various things */
	void setup(Texture* texture);
	void setup(Texture* texture, float width, float height);
	void setup(TextureAtlas* textureAtlas);
	void setup(TextureAtlas* texture, float width, float height);

	/* The destructor */
	virtual ~Sprite();

	/* Method called to update this sprite */
	virtual void update(float deltaSeconds);

	/* Method used to add an animation */
	inline void addAnimation(std::string name, Animation2D* animation) {
		animation->setSprite(this);
		animations.insert(std::pair<std::string, Animation2D*>(name, animation));
	}

	/* Method used to start an animation given its name */
	void startAnimation(std::string name);

	/* Method used to stop the current animation */
	void stopAnimation();

	/* Assigns the texture coordinates of this sprite for an index in a texture atlas */
	void setTextureCoords(TextureAtlas* textureAtlas, unsigned int index);

	/* Returns the name of the current animation (empty if there is none) */
	inline std::string getCurrentAnimationName() { return currentAnimationName; }
	/* Returns the current animation */
	inline Animation2D* getCurrentAnimation() { return currentAnimation; }
};

#endif /* CORE_SPRITE_H_ */
