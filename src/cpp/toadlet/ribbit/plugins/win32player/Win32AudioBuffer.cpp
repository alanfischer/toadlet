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
#include <toadlet/ribbit/AudioFormatConversion.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

Win32AudioBuffer::Win32AudioBuffer(Win32Player *player):BaseResource(),
	mAudioPlayer(NULL),
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

	int channels=decoder->getChannels();
	int sps=decoder->getSamplesPerSecond();
	int bps=decoder->getBitsPerSample();
	tbyte *buffer=NULL;
	int length=0;

	AudioFormatConversion::decode(decoder,buffer,length);
	int numsamps=length/channels/(bps/8);

	// Lets us programatically reduce popping on some platforms
	if(mAudioPlayer->getBufferFadeTime()>0){
		int stf=sps*mAudioPlayer->getBufferFadeTime()/1000;
		if(stf>numsamps){stf=numsamps;}
		int sampsize=channels*(bps/8);
		int i,j;
		if(bps==8){
			for(i=0;i<stf;++i){
				// Fade front
				for(j=0;j<sampsize;++j){
					buffer[i*sampsize+j]=(uint8)(((((int)buffer[i*sampsize+j])-128)*i/stf)+128);
				}
				// Fade back
				for(j=0;j<sampsize;++j){
					buffer[(numsamps-i-1)*sampsize+j]=(uint8)(((((int)buffer[(numsamps-i-1)*sampsize+j])-128)*i/stf)+128);
				}
			}
		}
		else if(bps==16){
			for(i=0;i<stf;++i){
				// Fade front
				for(j=0;j<sampsize;++j){
					buffer[i*sampsize+j]=(tbyte)(((int)buffer[i*sampsize+j])*i/stf);
				}
				// Fade back
				for(j=0;j<sampsize;++j){
					buffer[(numsamps-i-1)*sampsize+j]=(tbyte)(((int)buffer[(numsamps-i-1)*sampsize+j])*i/stf);
				}
			}
		}
	}

	int nchannels=mAudioPlayer->getChannels();	
	int nbps=mAudioPlayer->getBitsPerSample();
	int nsps=mAudioPlayer->getSamplesPerSecond();

	if(nchannels!=channels || nbps!=bps || nsps!=sps){
		Logger::debug(Categories::TOADLET_RIBBIT,String("converting audio from ")+channels+","+bps+","+sps+" to "+nchannels+","+nbps+","+nsps);

		int nlength=numsamps*nchannels*(nbps/8);
		tbyte *nbuffer=new tbyte[nlength];

		AudioFormatConversion::convert(buffer,channels,bps,sps,nbuffer,nchannels,nbps,nsps,length);

		delete[] buffer;

		mLength=nlength;
		mData=nbuffer;
	}
	else{
		mLength=length;
		mData=buffer;
	}

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
