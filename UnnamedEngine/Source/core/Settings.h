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

#ifndef CORE_SETTINGS_H_
#define CORE_SETTINGS_H_

#include "Vector.h"

/* Various predetermined video resolutions */
namespace VideoResolution {
	const Vector2i RES_640x480    = Vector2i(640, 480);
	const Vector2i RES_1280x720   = Vector2i(1280, 720);
	const Vector2i RES_1366x768   = Vector2i(1366, 768);
	const Vector2i RES_1920x1080  = Vector2i(1920, 1080);
	const Vector2i RES_3840x2160  = Vector2i(3840, 2160);

	/* The named video resolutions */
	const Vector2i RES_DEFAULT    = RES_1280x720;
	const Vector2i RES_720P       = RES_1280x720;
	const Vector2i RES_1080P      = RES_1920x1080;
	const Vector2i RES_4K         = RES_3840x2160;
}

/*****************************************************************************
 * The Settings structure holds data about various settings
 *****************************************************************************/

struct Settings {
	/* Various window settings */
	std::string  windowTitle      = "Unnamed Engine";
	unsigned int windowWidth      = 0;
	unsigned int windowHeight     = 0;
	bool         windowResizable  = false;
	bool         windowDecorated  = true;
	bool         windowBorderless = false;
	bool 		 windowFullscreen = false;
	bool         windowFloating   = false;

	/* Various video settings */
	bool         videoVSync       = false;
	unsigned int videoSamples     = 0;
	unsigned int videoMaxAnisotropicSamples = 0;
	unsigned int videoRefreshRate = 60;
	Vector2i     videoResolution  = VideoResolution::RES_DEFAULT;
	unsigned int videoMaxFPS      = 60;

	/* Various audio settings */
	unsigned int audioSoundEffectVolume = 100;
	unsigned int audioMusicVolume       = 100;

	/* Various input settings */
	bool         inputRepeatKeyboardEvents = true;
	bool         inputRepeatMouseEvents    = true;

	/* Various engine settings */
	bool         engineSplashScreen = false;

	/* Various debugging settings */
	bool         debuggingShowInformation = true;
};

#endif /* CORE_SETTINGS_H_ */
