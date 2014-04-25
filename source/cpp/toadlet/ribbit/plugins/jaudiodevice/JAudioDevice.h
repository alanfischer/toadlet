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

#ifndef TOADLET_RIBBIT_JAUDIODEVICE_H
#define TOADLET_RIBBIT_JAUDIODEVICE_H

#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/BaseAudioDevice.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioCaps.h>
#include <jni.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API JAudioDevice:public BaseAudioDevice{
public:
	TOADLET_IOBJECT(JAudioDevice);

	JAudioDevice(JNIEnv *jenv,jobject jobj);
	virtual ~JAudioDevice();

	bool create(int *options);
	void destroy();

	void activate(){}
	void deactivate(){}

	AudioBuffer *createAudioBuffer();
	Audio *createAudio();

	void setListenerTranslate(const Vector3 &translate){}
	void setListenerRotate(const Quaternion &rotate){}
	void setListenerVelocity(const Vector3 &velocity){}
	void setListenerGain(scalar gain){}

	void update(int dt);

	bool getAudioCaps(AudioCaps &caps){caps.set(mCaps);return true;}

protected:
	JNIEnv *getEnv() const;

	JavaVM *vm;
	jobject obj;
	jmethodID createID,destroyID,createAudioBufferID,createAudioID,updateID;

	AudioCaps mCaps;
};

}
}

#endif
