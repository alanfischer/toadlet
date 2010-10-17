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

#include "Win32Audio.h"
#include "Win32AudioBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/MathConversion.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

Win32Audio::Win32Audio(Win32Player *player):
	mPlayer(NULL),
	//mAudioBuffer,
	mPlaying(false),
	mLooping(false),
	mGain(Math::ONE),
	mPitch(Math::ONE),

	mPosition(0)
{
	mPlayer=player;
}

Win32Audio::~Win32Audio(){
	destroy();
}

bool Win32Audio::create(AudioBuffer::ptr audioBuffer){
	destroy();

	mPlayer->internal_audioCreate(this);
	mAudioBuffer=shared_static_cast<Win32AudioBuffer>(audioBuffer);
	return true;
}

bool Win32Audio::create(Stream::ptr stream,const String &mimeType){
	destroy();

	return false;
}

void Win32Audio::destroy(){
	mPlayer->internal_audioDestroy(this);
}

int Win32Audio::read(tbyte *data,int length){
	if(mPlaying==false){
		return 0;
	}

	int amount=Math::minVal(length,mAudioBuffer->mLength-mPosition);
	memcpy(data,mAudioBuffer->mData+mPosition,amount);
	mPosition+=amount;

	if(amount>0){
		int i;
		if(mGain!=Math::ONE){
			if(mPlayer->getBitsPerSample()==8){
				for(i=0;i<amount;++i){
					data[i]=Math::toInt(Math::mul(Math::fromInt(data[i]),mGain));
				}
			}
			else if(mPlayer->getBitsPerSample()==16){
				for(i=0;i<amount;++i){
					/// @todo: make this work on uin16 instead of just tbytes
					data[i]=Math::toInt(Math::mul(Math::fromInt(data[i]),mGain));
				}
			}
		}

		for(i=amount;i<length;++i){
			data[i]=data[amount-1];
		}
	}

	if(mPosition>=mAudioBuffer->mLength){
		if(mLooping){
			mPosition=0;
		}
		else{
			mPlaying=false;
		}
	}

	return amount;
}

}
}
