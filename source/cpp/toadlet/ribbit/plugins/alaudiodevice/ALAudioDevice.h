/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_RIBBIT_ALAUDIODEVICE_H
#define TOADLET_RIBBIT_ALAUDIODEVICE_H

#include "ALIncludes.h"
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/BaseAudioDevice.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioStream.h>
#include <toadlet/ribbit/AudioCaps.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Object.h>

namespace toadlet{
namespace ribbit{
	
class ALAudio;
class ALAudioBuffer;

typedef void (*proc_alBufferDataStatic)(ALuint buffer,ALenum format,ALvoid *data,ALsizei size,ALsizei freq);

// You can not use both the ALAUdioDevice and the MMAudioDevice simultaniously.
// Whichever one you create second will be the useable device.
class TOADLET_API ALAudioDevice:public BaseAudioDevice{
public:
	TOADLET_OBJECT(ALAudioDevice);

	// Options
	const static int Option_BUFFER_FADE_TIME=1;
	
	ALAudioDevice();
	virtual ~ALAudioDevice();

	bool create(int *options);
	void destroy();

	void activate();
	void deactivate();

	AudioBuffer *createAudioBuffer();
	Audio *createAudio();

	void setListenerTranslate(const Vector3 &translate);
	void setListenerRotate(const Quaternion &rotate);
	void setListenerVelocity(const Vector3 &velocity);
	void setListenerGain(scalar gain);

	void update(int dt);

	bool getAudioCaps(AudioCaps &caps){caps.set(mCaps);return true;}

	ALuint checkoutSourceHandle(ALAudio *audio);
	void checkinSourceHandle(ALAudio *audio,ALuint source);
	int getBufferFadeTime() const{return mBufferFadeTime;}

	static ALenum getALFormat(AudioFormat *format);

	egg::math::Vector3 cacheVector3;

protected:
	void preDestroyBuffer(ALAudioBuffer *buffer);

	ALCdevice *mDevice;
	static ALCcontext *mContext; // Only 1 Context is supported at a time in OpenAL, so we make it static
	Collection<ALAudio*> mAudios;
	Collection<unsigned int> mSourcePool;
	Collection<unsigned int> mAllSources;
	int mBufferFadeTime;

	AudioCaps mCaps;

	proc_alBufferDataStatic alBufferDataStatic;

	friend class ALAudioBuffer;
};

}
}

#endif

