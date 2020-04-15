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

#pragma once

#include "ParticleSystem.h"
#include "ParticleEmitter.h"

/*****************************************************************************
 * The ParticleEffect class can be inherited to modify how particles behave
 * over time
 *****************************************************************************/

class ParticleEffect {
public:
	/* The constructor */
	ParticleEffect() {}

	/* The destructor */
	virtual ~ParticleEffect() {}

	/* The method use to update this effect given a reference to a particle */
	virtual void update(Particle& particle, ParticleEmitter* emitter) {}
};

/*****************************************************************************
 * The ParticleEffectColourFade class is a particle effect that interpolates
 * between two colours over a particles lifetime
 *****************************************************************************/

class ParticleEffectColourChange : public ParticleEffect {
private:
	/* The start colour and end colour */
	Colour colourStart;
	Colour colourEnd;
public:

	/* The constructor */
	ParticleEffectColourChange(Colour colourStart, Colour colourEnd) : colourStart(colourStart), colourEnd(colourEnd) {}

	/* The destructor */
	virtual ~ParticleEffectColourChange() {}

	/* The method use to update this effect given a reference to a particle */
	virtual void update(Particle& particle, ParticleEmitter* emitter) override;

	/* Setters and getters */
	inline void setColourStart(Colour colour) { colourStart = colour; }
	inline void setColourEnd(Colour colour) { colourEnd = colour; }
	inline Colour& getColourStart() { return colourStart; }
	inline Colour& getColourEnd() { return colourEnd; }
};


