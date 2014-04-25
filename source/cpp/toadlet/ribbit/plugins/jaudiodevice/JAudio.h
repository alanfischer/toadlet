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

#ifndef TOADLET_RIBBIT_JAUDIO_H
#define TOADLET_RIBBIT_JAUDIO_H

#include "JAudioDevice.h"
#include "JAudioBuffer.h"
#include <toadlet/ribbit/BaseAudio.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API JAudio:public BaseAudio{
public:
	TOADLET_IOBJECT(JAudio);

	JAudio(JNIEnv *jenv,jobject jobj);
	virtual ~JAudio();

	bool create(AudioBuffer *audioBuffer);
	bool create(AudioStream *stream);
	void destroy();

	AudioBuffer *getAudioBuffer(){return mAudioBuffer;}
	AudioStream *getAudioStream(){return mAudioStream;}

	bool play();
	bool stop();
	bool getPlaying() const;
	bool getFinished() const;

	void setLooping(bool looping);
	bool getLooping() const;

	void setGain(scalar gain);
	scalar getGain() const;

protected:
	JNIEnv *getEnv() const;

	JavaVM *vm;
	jobject obj;
	jmethodID createAudioBufferID,createAudioStreamID,destroyID,playID,stopID,getPlayingID,getFinishedID,setLoopingID,getLoopingID,setGainID,getGainID;

	AudioBuffer::ptr mAudioBuffer;
	AudioStream::ptr mAudioStream;
};

}
}

#endif
