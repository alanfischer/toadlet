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

#include <toadlet/tadpole/AudioBufferData.h>
#include <toadlet/tadpole/handler/AudioBufferHandler.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::ribbit;

namespace toadlet{
namespace tadpole{
namespace handler{

AudioBufferHandler::AudioBufferHandler(AudioPlayer *player){
	mAudioPlayer=player;
}

void AudioBufferHandler::setAudioPlayer(AudioPlayer *player){
	mAudioPlayer=player;
}

Resource::ptr AudioBufferHandler::load(InputStream::ptr in,const ResourceHandlerData *handlerData){
	AudioBuffer::ptr audioBuffer=NULL;
	if(mAudioPlayer!=NULL){
		String mimeType;
		AudioBufferData *audioBufferData=(AudioBufferData*)handlerData;
		if(audioBufferData==NULL){
			Logger::warning(Categories::TOADLET_TADPOLE,
				"no valid AudioBufferData, audio may not be decoded properly");
		}
		else{
			mimeType=audioBufferData->mimeType;
		}
		audioBuffer=AudioBuffer::ptr(mAudioPlayer->createAudioBuffer());
		audioBuffer->create(in,mimeType);
	}
	return audioBuffer;
}

}
}
}
