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

#include "Colour.h"

/*****************************************************************************
 * The Colour class
 *****************************************************************************/

const Colour Colour::NONE       = Colour(0.0f, 0.0f, 0.0f, 0.0f);
const Colour Colour::BLACK      = Colour(0.0f, 0.0f, 0.0f, 1.0f);
const Colour Colour::GREY       = Colour(0.2f, 0.2f, 0.2f, 1.0f);
const Colour Colour::LIGHT_GREY = Colour(0.45f, 0.45f, 0.45f, 1.0f);
const Colour Colour::RED        = Colour(1.0f, 0.0f, 0.0f, 1.0f);
const Colour Colour::ORANGE     = Colour(1.0f, 0.6470588235294118f, 0.0f, 1.0f);
const Colour Colour::YELLOW     = Colour(1.0f, 1.0f, 0.0f, 1.0f);
const Colour Colour::PINK       = Colour(1.0f, 0.0f, 1.0f, 1.0f);
const Colour Colour::GREEN      = Colour(0.0f, 1.0f, 0.0f, 1.0f);
const Colour Colour::BLUE       = Colour(0.0f, 0.0f, 1.0f, 1.0f);
const Colour Colour::LIGHT_BLUE = Colour(0.0f, 1.0f, 1.0f, 1.0f);
const Colour Colour::WHITE      = Colour(1.0f, 1.0f, 1.0f, 1.0f);

//const Colour Colour::ARRAY_RGB[]     = { Colour::RED, Colour::GREEN, Colour::BLUE    };
//const Colour Colour::ARRAY_GREY[]    = { Colour::GREY, Colour::LIGHT_GREY            };
//const Colour Colour::ARRAY_BLUE[]    = { Colour::BLUE, Colour::LIGHT_BLUE            };
//const Colour Colour::ARRAY_SUNSET[]  = { Colour::RED, Colour::ORANGE, Colour::YELLOW };


