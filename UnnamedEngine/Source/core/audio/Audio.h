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

#ifndef CORE_AUDIO_AUDIO_H_
#define CORE_AUDIO_AUDIO_H_

#include <AL/al.h>
#include <AL/alc.h>

#include "../Object.h"

/***************************************************************************************************
 * The AudioObject class
 ***************************************************************************************************/

class AudioObject {
private:
	/* The velocity for this audio object */
	Vector3f velocity;
protected:
	/* The transform of this audio object */
	Transform* transform = new Transform();
public:

	/* The default constructor */
	AudioObject() {}

	/* The various other constructors */
	AudioObject(Vector2f position) { setPosition(position); }
	AudioObject(Vector3f position) { setPosition(position); }
	AudioObject(Vector3f position, Vector3f rotation) { setPosition(position); setRotation(rotation); }
	AudioObject(Vector3f position, Vector3f rotation, Vector3f velocity) : velocity(velocity) { setPosition(position); setRotation(rotation); }

	AudioObject(GameObject* parent) { transform->setParent(parent->getTransform()); }

	/* Method used set the parent of this object */
	inline void setParent(GameObject* parent) { transform->setParent(parent->getTransform()); }
	/* method used to remove a child object */
	inline void removeChild(GameObject* child) { transform->removeChild(child->getTransform()); }

	/* The setters and getters */
	inline void setPosition(Vector3f position) { transform->setPosition(position); }
	inline void setPosition(float x, float y, float z)  { transform->setPosition(x, y, z); }
	inline void setRotation(Quaternion rotation) { transform->setRotation(rotation); }
	inline void setRotation(Vector3f rotation) { transform->setRotation(rotation); }
	inline void setRotation(float x, float y, float z) { transform->setRotation(x, y, z); }
	inline void setVelocity(Vector3f velocity) { this->velocity = velocity; }
	inline void setVelocity(float x, float y, float z) { this->velocity = Vector3f(x, y, z); }

	inline Vector3f   getLocalPosition()      { return transform->getLocalPosition(); }
	inline Quaternion getLocalRotation()      { return transform->getLocalRotation(); }
	inline Vector3f   getLocalRotationEuler() { return transform->getLocalRotation().toEuler(); }

	inline Vector3f   getPosition()      { return transform->getPosition(); }
	inline Quaternion getRotation()      { return transform->getRotation(); }
	inline Vector3f   getRotationEuler() { return transform->getRotation().toEuler(); }
	inline Vector3f   getVelocity()      { return velocity; }

	inline Transform* getTransform() { return transform; }
};

/*****************************************************************************
 * The AudioData structure stores audio data for OpenAL
 *****************************************************************************/

struct AudioData {
	/* The size of the audio data in bytes */
	ALsizei  size;
	/* The sample rate / frequency of the audio data */
	ALsizei  frequency;
	/* The format of the audio data */
	ALenum   format;
	/* The audio data stored */
	short* data;
};

/*****************************************************************************
 * The AudioLoader class stores everything necessary / has methods to load
 * Audio files to return an AudioData structure
 *
 * http://soundfile.sapp.org/doc/WaveFormat/
 *****************************************************************************/

class AudioLoader {
private:
	/* Structures for loading Wave files */

	/* The structure for the "RIFF" chunk descriptor */
	struct Wave_RIFF {
		char ChunkID[4]; //4 Bytes
		long ChunkSize;  //4 Bytes
		char Format[4];  //4 Bytes
	};

	/* The structure for the "fmt" sub-chunk */
	struct Wave_fmt {
		char  SubChunkID[4]; //4 Bytes
		long  SubChunkSize;  //4 Bytes
		short AudioFormat;   //2 Bytes
		short NumChannels;   //2 Bytes
		long  SampleRate;    //4 Bytes
		long  ByteRate;      //4 Bytes
		short BlockAllign;   //2 Bytes
		short BitsPerSample; //2 Bytes
	};

	/* The structure for the "data" sub-chunk */
	struct Wave_data {
		char SubChunkID[4]; //4 Bytes
		long SubChunkSize;  //4 Bytes
		//Data would be stored here, but size is not known until SubChunkSize
		//is known and after that its contents could just go straight into
		//AudioData anyway
	};
public:
	/* Method used to load the AudioData for an audio file */
	static AudioData* loadFile(std::string path);
	/* Method used to load the AudioData for a Wave file */
	static AudioData* loadWaveFile(std::string path);
	/* Method used to load the AudioData for an Ogg file */
	static AudioData* loadOggFile(std::string path);
};

/***************************************************************************************************
 * The AudioSource class
 ***************************************************************************************************/

class AudioSource : public AudioObject {
private:
	ALuint sourceHandle = 0;
	ALuint bufferHandle = 0;

	unsigned int type = 0;

	float pitch = 0;
	float gain  = 0;

	bool loop = false;
public:
	static const unsigned int TYPE_SOUND_EFFECT = 1;
	static const unsigned int TYPE_MUSIC        = 2;

	/* The constructors */
	AudioSource(AudioData* data, unsigned int type);
	AudioSource(AudioData* data, unsigned int type, Vector2f position) : AudioSource(data, type) {
		setPosition(position);
	}
	AudioSource(AudioData* data, unsigned int type, Vector3f position) : AudioSource(data, type) {
		setPosition(position);
	}
	AudioSource(AudioData* data, unsigned int type, GameObject* parent) : AudioSource(data, type) {
		transform->setParent(parent->getTransform());
	}

	virtual ~AudioSource() {
		destroy();
	}

	void updateVolume();

	void update();

	/* Various audio related methods */
	void play();
	void stop();
	void pause();
	void resume();
	void destroy();

	/* The setters and getters */
	inline void setPitch(float pitch) { this->pitch = pitch; }
	inline void setGain(float gain) { this->gain = gain; }
	inline void setLoop(bool loop) { this->loop = loop; }
	inline float getPitch() { return pitch; }
	inline float getGain() { return gain; }
	inline bool doesLoop() { return loop; }
	bool isPlaying();
	bool isPaused();
};

/***************************************************************************************************
 * The AudioListener class
 ***************************************************************************************************/

class AudioListener : public AudioObject {
public:
	/* The default constructor */
	AudioListener() {}

	/* The other constructors */
	AudioListener(GameObject* parent) : AudioObject(parent) {}
	AudioListener(Vector2f position) : AudioObject(position) {}
	AudioListener(Vector3f position) : AudioObject(position) {}

	/* The method used to update this listener */
	void update();
};

/***************************************************************************************************
 * The AudioManager class
 ***************************************************************************************************/

class AudioManager {
private:
	/* All of the created audio sources */
	static std::vector<AudioSource*> sources;
	/* The audio context and device */
	static ALCcontext* context;
	static ALCdevice* device;
public:
	/* Method to add an audio source */
	static void add(AudioSource* source);
	/* Method to remove an audio source */
	static void remove(AudioSource* source);
	/* Method to initialise OpenAL */
	static void initialise();
	/* Method to delete all sources and the OpenAL context */
	static void destroy();
	/* Method to update the volume of all audio sources */
	static void updateVolume();

	static inline bool hasContext() { return context != NULL; }
};

#endif /* CORE_AUDIO_AUDIO_H_ */
