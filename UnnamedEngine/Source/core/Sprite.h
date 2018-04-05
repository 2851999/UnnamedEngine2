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

/*****************************************************************************
 * The Animation2D class helps to create 2D animations
 *****************************************************************************/

class Animation2D {
protected:
	/* The entity to apply this animation to */
	GameObject2D* entity = NULL;

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

	/* The current time for the animation */
	float currentTime = 0.0f;
public:
	/* The constructors */
	Animation2D(GameObject2D* entity, float timeBetweenFrame, unsigned int totalFrames, bool repeat = false);
	Animation2D(float timeBetweenFrame, unsigned int totalFrames, bool repeat = false) : Animation2D(NULL, timeBetweenFrame, totalFrames, repeat) {}

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
	inline void setEntity(GameObject2D* entity) { this->entity = entity; }

	inline float getTimeBetweenFrame() { return timeBetweenFrame; }
	inline bool  doesRepeat() { return repeat; }
	inline bool  isRunning()  { return running; }
	inline GameObject2D* getEntity() { return entity; }
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
	TextureAnimation2D(GameObject2D* entity, TextureAtlas* textureAtlas, float timeBetweenFrame, bool repeat = false);
	TextureAnimation2D(TextureAtlas* textureAtlas, float timeBetweenFrame, bool repeat = false) : TextureAnimation2D(NULL, textureAtlas, timeBetweenFrame, repeat) {}

	/* Method called when the frame needs to change */
	virtual void updateFrame() override;

	/* Called when this animation starts */
	virtual void onStart();
};

/*****************************************************************************
 * The Sprite2D class helps to create a 2D sprite that can have animations
 * attached to them
 *****************************************************************************/

class Sprite2D : public GameObject2D {
private:
	/* The animations for this sprite */
	std::unordered_map<std::string, Animation2D*> animations;

	/* The current animation being played */
	Animation2D* currentAnimation = NULL;
public:
	/* The constructors */
	Sprite2D(Texture* texture);
	Sprite2D(Texture* texture, float width, float height);

	/* The destructor */
	virtual ~Sprite2D();

	/* Method called to update this sprite */
	void update(float deltaSeconds);

	/* Method used to add an animation */
	inline void addAnimation(std::string name, Animation2D* animation) {
		animation->setEntity(this);
		animations.insert(std::pair<std::string, Animation2D*>(name, animation));
	}

	/* Method used to start an animation given its name */
	void startAnimation(std::string name);

	/* Method used to stop the current animation */
	void stopAnimation();
};

#endif /* CORE_SPRITE_H_ */
