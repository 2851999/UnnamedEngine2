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

#include "Settings.h"

namespace VideoResolution {
	/* Used to convert a Vector2i into a string representing a resolution */
	std::string toString(Vector2i resolution) {
		return utils_string::str(utils_string::str(resolution.getX()) + " x " + utils_string::str(resolution.getY()));
	}

	/* Used to convert a string into a Vector2i representing a resolution */
	Vector2i toVector(std::string resolution) {
		std::vector<std::string> split = utils_string::strSplit(resolution, " x ");
		return Vector2i(utils_string::strToInt(split[0]), utils_string::strToInt(split[1]));
	}
}


