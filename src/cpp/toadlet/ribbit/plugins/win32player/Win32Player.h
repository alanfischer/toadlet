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

#ifndef TOADLET_RIBBIT_WIN32PLAYER_H
#define TOADLET_RIBBIT_WIN32PLAYER_H

#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioStream.h>
#include <toadlet/ribbit/CapabilitySet.h>
#include <toadlet/egg/Collection.h>
#include <windows.h>
#include <mmsystem.h>

namespace toadlet{
namespace ribbit{

class Win32Audio;
class Win32AudioBuffer;

class TOADLET_API Win32Player:public AudioPlayer{
public:
	Win32Player();
	virtual ~Win32Player();

	bool create(int *options);
	bool destroy();

	AudioBuffer *createAudioBuffer();
	Audio *createBufferedAudio();
	Audio *createStreamingAudio();

	void setListenerTranslate(const Vector3 &translate){}
	void setListenerRotate(const Matrix3x3 &rotate){}
	void setListenerVelocity(const Vector3 &velocity){}
	void setListenerGain(scalar gain){}

	void suspend(){}
	void resume(){}

	void update(int dt);

	const CapabilitySet &getCapabilitySet(){return mCapabilitySet;}

	AudioStream::ptr startAudioStream(egg::io::Stream::ptr stream,const egg::String &mimeType);
	void decodeStream(AudioStream *decoder,tbyte *&finalBuffer,int &finalLength);

	void internal_audioCreate(Win32Audio *audio);
	void internal_audioDestroy(Win32Audio *audio);

protected:
	int read(int8 *data,int length);

	CapabilitySet mCapabilitySet;

	egg::Collection<Win32Audio*> mAudios;

	int mChannels;
	int mSamplesPerSecond;
	int mBitsPerSample;

	HWAVEOUT mDevice;
	WAVEHDR *mBuffers;
	int16 *mBufferData;
};

}
}

#endif
