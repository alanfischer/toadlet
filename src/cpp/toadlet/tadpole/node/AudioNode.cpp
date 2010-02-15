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

#include <toadlet/tadpole/node/AudioNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::ribbit;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(AudioNode,Categories::TOADLET_TADPOLE_NODE+".AudioNode");

AudioNode::AudioNode():super(){}

Node *AudioNode::create(Engine *engine){
	super::create(engine);
	return this;
}

void AudioNode::destroy(){
	if(mAudio!=NULL){
		mAudio->destroy();
		mAudio=NULL;
	}

	super::destroy();
}

bool AudioNode::setAudioBuffer(const AudioBuffer::ptr &audioBuffer){
	if(mAudio!=NULL){
		mAudio->destroy();
	}

	mAudio=Audio::ptr(mEngine->getAudioPlayer()->createBufferedAudio());
	if(mAudio!=NULL){
		mAudio->create(audioBuffer);
		return true;
	}
	else{
		return false;
	}
}

bool AudioNode::setAudioStream(Stream::ptr stream,const String &mimeType){
	if(mAudio!=NULL){
		mAudio->destroy();
	}

	mAudio=Audio::ptr(mEngine->getAudioPlayer()->createStreamingAudio());
	if(mAudio!=NULL){
		mAudio->create(stream,mimeType);
		return true;
	}
	else{
		return false;
	}
}

}
}
}
