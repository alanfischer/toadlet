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

#ifndef TOADLET_TADPOLE_AUDIOSTREAMER_H
#define TOADLET_TADPOLE_AUDIOSTREAMER_H

#include <toadlet/tadpole/ResourceStreamer.h>
#include <toadlet/tadpole/AudioManager.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API AudioStreamer:public ResourceStreamer{
public:
	TOADLET_INTERFACE(AudioStreamer);

	AudioStreamer(AudioManager *manager){mAudioManager=manager;}

	virtual AudioStream::ptr createAudioStream(Stream *stream,ResourceData *data)=0;

	bool load(Stream::ptr stream,ResourceData *data,ResourceRequest *request){
		AudioBuffer::ptr result;
		Exception exception;
		TOADLET_TRY
			result=mAudioManager->createAudioBuffer(createAudioStream(stream,data));
		TOADLET_CATCH(Exception ex){exception=ex;}
		if(result!=NULL){
			request->resourceReady(result);
			return true;
		}
		else{
			request->resourceException(exception);
			return false;
		}
	}

	bool save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ResourceRequest *request){return false;}

protected:
	AudioManager *mAudioManager;
};

}
}

#endif

