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

#ifndef EXAMPLES_ASTEROIDS_ASTEROIDSRESOURCES_H_
#define EXAMPLES_ASTEROIDS_ASTEROIDSRESOURCES_H_

#include "../../core/gui/Font.h"
#include "../../core/render/Texture.h"
#include "../../core/ResourceLoader.h"

/*****************************************************************************
 * The AsteroidsResources class stores some of the commonly used resources
 *****************************************************************************/

class AsteroidsResources {
private:
	/* Various fonts */
	Font* fontTitle = NULL;
	Font* fontGUI = NULL;
	Font* fontHeading = NULL;
	Font* fontHeadingMono = NULL;

	/* Various textures */
	std::vector<Texture*> texturesButtons;
	Texture* textureShieldBar = NULL;
	Texture* textureCrossHair = NULL;

	/* Various sounds */
	AudioData* audioLaser = NULL;
	AudioData* audioExplosion = NULL;
	AudioData* audioBackground = NULL;
public:
	/* The constructor */
	AsteroidsResources();

	/* The destructor */
	virtual ~AsteroidsResources();

	/* Method called to create the resources */
	void setup(ResourceLoader& loader);

	/* Various getter methods */
	inline Font* getFontTitle() const { return fontTitle; }
	inline Font* getFontGUI() const { return fontGUI; }
	inline Font* getFontHeading() const { return fontHeading; }
	inline Font* getFontHeadingMono() const { return fontHeadingMono; }

	inline const std::vector<Texture*>& getTexturesButtons() const { return texturesButtons; }
	inline Texture* getTextureShieldBar() const { return textureShieldBar; }
	inline Texture* getTextureCrossHair() const { return textureCrossHair; }

	inline AudioData* getAudioLaser() const { return audioLaser; }
	inline AudioData* getAudioExplosion() const { return audioExplosion; }
	inline AudioData* getAudioBackground() const { return audioBackground; }
};

#endif /* EXAMPLES_ASTEROIDS_ASTEROIDSRESOURCES_H_ */
