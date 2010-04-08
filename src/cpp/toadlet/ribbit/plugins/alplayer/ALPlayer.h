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

#ifndef TOADLET_RIBBIT_ALPLAYER_H
#define TOADLET_RIBBIT_ALPLAYER_H

#include "ALIncludes.h"
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioStream.h>
#include <toadlet/ribbit/CapabilitySet.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace ribbit{
	
class ALAudio;
#if defined(TOADLET_PLATFORM_OSX)
	class CoreAudio;
#endif

typedef void (*proc_alBufferDataStatic)(ALuint buffer,ALenum format,ALvoid *data,ALsizei size,ALsizei freq);

class TOADLET_API ALPlayer:public AudioPlayer,public egg::Runnable{
public:
	// Options
	const static int Option_FADE_IN_BUFFER_TIME=1;
	
	ALPlayer();
	virtual ~ALPlayer();

	bool create(int *options);
	bool destroy();

	AudioBuffer *createAudioBuffer();
	Audio *createBufferedAudio();
	Audio *createStreamingAudio();

	void suspend();
	void resume();

	void setListenerTranslate(const Vector3 &translate);
	void setListenerRotate(const Matrix3x3 &rotate);
	void setListenerVelocity(const Vector3 &velocity);
	void setListenerGain(scalar gain);

	void setDopplerFactor(scalar factor);
	void setDopplerVelocity(scalar velocity);
	void setDefaultRolloffFactor(scalar factor);

	const CapabilitySet &getCapabilitySet();

	// Methods not part of the AudioPlayer interface
	AudioStream::ptr startAudioStream(egg::io::Stream::ptr stream,const egg::String &mimeType);
	void decodeStream(AudioStream *decoder,byte *&finalBuffer,int &finalLength);
	ALuint checkoutSourceHandle(ALAudio *audio);
	void checkinSourceHandle(ALAudio *audio,ALuint source);
	int getBufferFadeInTime() const{return mBufferFadeInTime;}

	void lock(){mMutex.lock();}
	void unlock(){mMutex.unlock();}

	static ALenum getALFormat(int bitsPerSample,int channels);

	egg::math::Vector3 cacheVector3;

	#if defined(TOADLET_PLATFORM_OSX)
		void registerCoreAudio(CoreAudio *audio){mCoreAudios.add(audio);}
		void unregisterCoreAudio(CoreAudio *audio){mCoreAudios.remove(audio);}
	#endif
	
protected:
	void run();

	ALCdevice *mDevice;
	ALCcontext *mContext;
	egg::Collection<ALAudio*> mAudios;
	egg::Collection<unsigned int> mSourcePool;
	egg::Collection<unsigned int> mAllSources;
	scalar mDefaultRolloffFactor;
	int mBufferFadeInTime;

	bool mStopThread;
	egg::Thread::ptr mThread;
	egg::Mutex mMutex;

	CapabilitySet mCapabilitySet;

	#if defined(TOADLET_PLATFORM_OSX)
		egg::Collection<CoreAudio*> mCoreAudios;
	#endif

	proc_alBufferDataStatic alBufferDataStatic;

	friend class ALAudioBuffer;
};

}
}

#endif

