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

#ifndef TOADLET_TADPOLE_MODSTREAMER_H
#define TOADLET_TADPOLE_MODSTREAMER_H

#include <toadlet/tadpole/AudioStreamer.h>
#include "MODDecoder.h"

namespace toadlet{
namespace tadpole{

class TOADLET_API MODStreamer:public Object,public AudioStreamer{
public:
	TOADLET_OBJECT(MODStreamer);

	MODStreamer(AudioManager *audioManager):AudioStreamer(audioManager){}

	AudioStream::ptr createAudioStream(Stream *stream,ResourceData *data){
		AudioData *audioData=(AudioData*)data;

		MODDecoder::ptr audioStream=new MODDecoder();
		if(audioStream && audioStream->openStream(stream)==false){
			audioStream=NULL;
		}
		return audioStream;
	}
};

}
}

#endif

