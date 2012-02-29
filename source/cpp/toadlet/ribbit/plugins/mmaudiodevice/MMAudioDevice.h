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

#ifndef TOADLET_RIBBIT_MMAUDIODEVICE_H
#define TOADLET_RIBBIT_MMAUDIODEVICE_H

#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioDevice.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioStream.h>
#include <toadlet/ribbit/AudioCaps.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Object.h>
#include <windows.h>
#include <mmsystem.h>

namespace toadlet{
namespace ribbit{

class MMAudio;
class MMAudioBuffer;

// You can not use both the ALAUdioDevice and the MMAudioDevice simultaniously.
// Whichever one you create second will be the useable device.
class TOADLET_API MMAudioDevice:protected Object,public AudioDevice{
public:
	TOADLET_OBJECT(MMAudioDevice);

	// Options
	const static int Option_BUFFER_FADE_TIME=1;
	
	MMAudioDevice();
	virtual ~MMAudioDevice();

	bool create(int *options);
	void destroy();

	void activate(){}
	void deactivate(){}

	AudioBuffer *createAudioBuffer();
	Audio *createAudio();

	void setListenerTranslate(const Vector3 &translate){}
	void setListenerRotate(const Matrix3x3 &rotate){}
	void setListenerVelocity(const Vector3 &velocity){}
	void setListenerGain(scalar gain){}

	void update(int dt);

	bool getAudioCaps(AudioCaps &caps){caps.set(mCaps);return true;}

	int getBufferFadeTime() const{return mBufferFadeTime;}
	AudioFormat::ptr getAudioFormat(){return mFormat;}

	void internal_audioCreate(MMAudio *audio);
	void internal_audioDestroy(MMAudio *audio);

protected:
	int read(tbyte *data,int length);

	AudioCaps mCaps;

	AudioFormat::ptr mFormat;
	HWAVEOUT mDevice;
	WAVEHDR *mBuffers;
	tbyte *mBufferData;
	Collection<MMAudio*> mAudios;
	int mBufferFadeTime;
	int mNumBuffers;
	int mBufferSize;
};

}
}

#endif
