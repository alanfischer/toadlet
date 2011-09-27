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

#ifndef TOADLET_RIBBIT_JATAUDIODEVICE_H
#define TOADLET_RIBBIT_JATAUDIODEVICE_H

#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioDevice.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioCaps.h>
#include <jnih>

namespace toadlet{
namespace ribbit{

/// @todo: Once all objects are internally reference counted, then switch JATAudioDevice to just a JAudioDevice
class TOADLET_API JATAudioDevice:public AudioDevice{
public:
	JATAudioDevice(JNIEnv *jenv,jobject jobj);
	virtual ~JATAudioDevice();

	bool create(int *options);
	void destroy();

	AudioBuffer *createAudioBuffer();
	Audio *createBufferedAudio();
	Audio *createStreamingAudio();

	void setListenerTranslate(const Vector3 &translate){}
	void setListenerRotate(const Matrix3x3 &rotate){}
	void setListenerVelocity(const Vector3 &velocity){}
	void setListenerGain(scalar gain){}

	void suspend(){}
	void resume(){}

	void update(int dt){}

	bool getAudioCaps(AudioCaps &caps){caps.set(mCaps);return true;}

	int getBufferFadeTime() const{return mBufferFadeTime;}
	AudioFormat::ptr getAudioFormat(){return mFormat;}

protected:
	JNIEnv *env;
	jobject obj;

	AudioCaps mCaps;
};

}
}

#endif
