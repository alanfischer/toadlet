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

#include "MMAudio.h"
#include "MMAudioBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/MathConversion.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

namespace toadlet{
namespace ribbit{

MMAudio::MMAudio(MMAudioDevice *device):
	mDevice(NULL),
	//mAudioBuffer,
	mPlaying(false),
	mLooping(false),
	mGain(Math::ONE),
	mPitch(Math::ONE),

	mPosition(0)
{
	mDevice=device;
}

bool MMAudio::create(AudioBuffer *audioBuffer){
	mDevice->internal_audioCreate(this);

	mAudioStream=NULL;
	mAudioBuffer=audioBuffer;

	return true;
}

bool MMAudio::create(AudioStream *stream){
	mDevice->internal_audioCreate(this);

	mAudioStream=stream;
	mAudioBuffer=NULL;

	return false;
}

void MMAudio::destroy(){
	mDevice->internal_audioDestroy(this);

	BaseResource::destroy();
}

int MMAudio::read(tbyte *data,int length){
	if(mPlaying==false){
		return 0;
	}

	int amount=0;
	if(mAudioStream!=NULL){
		AudioFormat *srcFormat=mAudioStream->getAudioFormat();
		AudioFormat *dstFormat=mDevice->getAudioFormat();

		if(srcFormat->equals(dstFormat)==false){
			// For the original src length we want from the dst length
			int dstLength=length;
			int srcLength=AudioFormatConversion::findConvertedLength(dstLength,dstFormat,srcFormat,false);
			tbyte *srcData=new tbyte[srcLength];
			amount=mAudioStream->read(srcData,srcLength);
			if(amount<0){
				if(mLooping){
					mAudioStream->reset();
					amount=mAudioStream->read(srcData,srcLength);
				}
				else{
					mPlaying=false;
				}
			}
			AudioFormatConversion::convert(srcData,amount,srcFormat,data,dstLength,dstFormat);
			amount=AudioFormatConversion::findConvertedLength(amount,srcFormat,dstFormat,true);
			delete[] srcData;
		}
		else{
			amount=mAudioStream->read(data,length);
			if(amount<0){
				if(mLooping){
					mAudioStream->reset();
					amount=mAudioStream->read(data,length);
				}
				else{
					mPlaying=false;
				}
			}
		}
	}
	else if(mAudioBuffer!=NULL){
		MMAudioBuffer *mmaudioBuffer=(MMAudioBuffer*)mAudioBuffer->getRootAudioBuffer();
		amount=Math::minVal(length,mmaudioBuffer->mLength-mPosition);
		memcpy(data,mmaudioBuffer->mData+mPosition,amount);
		mPosition+=amount;

		if(mPosition>=mmaudioBuffer->mLength){
			if(mLooping){
				mPosition=0;
			}
			else{
				mPlaying=false;
			}
		}
	}

	if(amount>0){
		int i;
		if(mGain<Math::ONE){
			int bps=mDevice->getAudioFormat()->getBitsPerSample();
			if(bps==8){
				for(i=0;i<amount;++i){
					((uint8*)data)[i]=Math::toInt(Math::mul(Math::fromInt(((int)((uint8*)data)[i])-128),mGain))+128;
				}
			}
			else if(bps==16){
				for(i=0;i<amount/2;++i){
					((int16*)data)[i]=Math::toInt(Math::mul(Math::fromInt(((int)((int16*)data)[i])),mGain));
				}
			}
		}

		for(i=amount;i<length;++i){
			data[i]=data[amount-1];
		}
	}

	return amount;
}

}
}
