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

#include "SIDDecoder.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

#if defined(SIDPLAY1)
#else
	#pragma comment(lib,"libsidplay2.lib")
	#pragma comment(lib,"resid_builder.lib")
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

SIDDecoder::SIDDecoder():
	mTune(0)
{}

SIDDecoder::~SIDDecoder(){
}

bool SIDDecoder::startStream(Stream::ptr stream){
	bool result=false;
	tbyte *tuneBuffer=NULL;
	int tuneBufferLength=0;
	int song=0;

	AudioFormatConversion::decode(stream,tuneBuffer,tuneBufferLength);
#if defined(SIDPLAY1)
	result=mTune.load(tuneBuffer,tuneBufferLength);
#else
	result=mTune.read(tuneBuffer,tuneBufferLength);
#endif
	delete[] tuneBuffer;
	if(result==false){
		return false;
	}

#if defined(SIDPLAY1)
	mPlayer.getConfig(mConfig);

	mConfig.sampleFormat=SIDEMU_UNSIGNED_PCM;
	mConfig.bitsPerSample=16;

	mPlayer.setConfig(mConfig);

	result=sidEmuInitializeSong(mPlayer,mTune,song);
	if(result==false){
		Error::unknown(Categories::TOADLET_RIBBIT,"Error initializing song");
		return false;
	}
#else
	mConfig=mPlayer.config();
	mInfo=mPlayer.info();

	ReSIDBuilder *resid=new ReSIDBuilder("ReSID");
	resid->create(mInfo.maxsids);
	resid->sampling(getSamplesPerSecond());
	mConfig.sidEmulation=resid;
	mConfig.frequency=getSamplesPerSecond();
	mConfig.precision=getBitsPerSample();
	mConfig.playback=(getChannels()==2?sid2_stereo:sid2_mono);

	mPlayer.config(mConfig);

	mTune.selectSong(song);

	result=(mPlayer.load(&mTune)>=0);
	if(result==false){
		Error::unknown(Categories::TOADLET_RIBBIT,mPlayer.error());
		return false;
	}

	mPlayer.fastForward(100);
#endif

	return result;
}

#if defined(SIDPLAY1)
	int SIDDecoder::getBitsPerSample(){return mConfig.bitsPerSample;}
	int SIDDecoder::getChannels(){return mConfig.channels;}
	int SIDDecoder::getSamplesPerSecond(){return mConfig.frequency;}
	int SIDDecoder::read(tbyte *buffer,int length){sidEmuFillBuffer(mPlayer,mTune,buffer,length);return length;}
#else
	int SIDDecoder::getBitsPerSample(){return mConfig.precision;}
	int SIDDecoder::getChannels(){return mInfo.channels;}
	int SIDDecoder::getSamplesPerSecond(){return mConfig.frequency;}
	int SIDDecoder::read(tbyte *buffer,int length){return mPlayer.play(buffer,length);}
#endif

}
}
