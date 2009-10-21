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
	
class ALAudioPeer;
#if defined(TOADLET_PLATFORM_OSX)
	class CoreAudioPeer;
#endif

typedef void (*proc_alBufferDataStatic)(ALuint buffer,ALenum format,ALvoid *data,ALsizei size,ALsizei freq);
extern proc_alBufferDataStatic toadlet_alBufferDataStatic;

class TOADLET_API ALPlayer:public AudioPlayer,public egg::Runnable{
public:
	// Options
	const static int Option_FADE_IN_BUFFER_TIME=1;
	
	ALPlayer();
	virtual ~ALPlayer();

	bool startup(int *options);
	bool shutdown();
	bool isStarted() const{return mStarted;}

	AudioBufferPeer *createAudioBufferPeer(AudioBuffer *audioBuffer);
	AudioPeer *createBufferedAudioPeer(Audio *audio,AudioBuffer::ptr buffer);
	AudioPeer *createStreamingAudioPeer(Audio *audio,egg::io::InputStream::ptr in,const egg::String &mimeType);

	void update(int dt); // milliseconds

	void suspend();
	void resume();

	void setListenerTranslate(const Vector3 &translate);
	void setListenerRotate(const Matrix3x3 &rotate);
	void setListenerVelocity(const Vector3 &velocity);
	void setListenerGain(scalar gain);

	void setDopplerFactor(scalar factor);
	void setDopplerVelocity(scalar velocity);

	void setDefaultRolloffFactor(scalar factor);
	scalar getDefaultRolloffFactor(){return mDefaultRolloffFactor;}

	void setGroupGain(const egg::String &group,scalar gain);
	scalar getGroupGain(const egg::String &group);
	void removeGroup(const egg::String &group);

	const CapabilitySet &getCapabilitySet();

	// Methods not part of the AudioPlayer interface
	void update(ALAudioPeer *audioPeer,int dt);
#	if defined(TOADLET_PLATFORM_OSX)
		void update(CoreAudioPeer *audioPeer,int dt);
#	endif
	AudioStream::ptr startAudioStream(egg::io::InputStream::ptr in,const egg::String &mimeType);

	void lock(){mMutex.lock();}
	void unlock(){mMutex.unlock();}

	scalar internal_getGroupGain(const egg::String &group);
	void internal_audioPeerDestroy(ALAudioPeer *audioPeer);
#	if defined(TOADLET_PLATFORM_OSX)
		void internal_audioPeerDestroy(CoreAudioPeer *audioPeer);
#	endif

	egg::math::Vector3 cacheVector3;

protected:
	void run();

	void decodeStream(AudioStream *decoder,char *&finalBuffer,int &finalLength);
	int readAudioData(ALAudioPeer *peer,unsigned char *buffer,int bsize);

	static ALenum getALFormat(int bitsPerSample,int channels);

	ALCdevice *mDevice;
	ALCcontext *mContext;
	egg::Collection<ALAudioPeer*> mAudioPeers;
	egg::Collection<unsigned int> mSourcePool;
	egg::Collection<unsigned int> mAllSources;
	scalar mDefaultRolloffFactor;
	int mBufferFadeInTime;

	egg::Map<egg::String,scalar> mGainGroup;
	bool mStopThread;
	egg::Thread *mThread;
	egg::Mutex mMutex;

	CapabilitySet mCapabilitySet;

#	if defined(TOADLET_PLATFORM_OSX)
		egg::Collection<CoreAudioPeer*> mCoreAudioPeers;
#	endif

	const static int bufferSize=4096*4;
	const static int numBuffers=8;

	bool mStarted;
};

}
}

#endif

