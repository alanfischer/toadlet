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

#include "Win32AudioBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

Win32AudioBuffer::Win32AudioBuffer(Win32Player *player):BaseResource(),
	mAudioPlayer(NULL),
	mChannels(0),
	mSamplesPerSecond(0),
	mBitsPerSample(0),
	mData(NULL),
	mLength(0)
{
	mAudioPlayer=player;
}

Win32AudioBuffer::~Win32AudioBuffer(){
	destroy();
}

bool Win32AudioBuffer::create(Stream::ptr stream,const String &mimeType){
	AudioStream::ptr decoder=mAudioPlayer->startAudioStream(stream,mimeType);
	if(decoder==NULL){
		return false;
	}

	mChannels=decoder->getChannels();
	mSamplesPerSecond=decoder->getSamplesPerSecond();
	mBitsPerSample=decoder->getBitsPerSample();
	mAudioPlayer->decodeStream(decoder,mData,mLength);

	return true;
}

void Win32AudioBuffer::destroy(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

}
}
