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

#if SIDPLAY_VERSION==1
	#include <sidplay/player.h>
#elif SIDPLAY_VERSION==2
	#include <sidplay/sidplay2.h>
	#include <sidplay/builders/resid.h>
#endif

#if defined(TOADLET_LIBSIDPLAY_NAME)
	#pragma comment(lib,TOADLET_LIBSIDPLAY_NAME)
#endif
#if defined(TOADLET_LIBRESID_NAME)
	#pragma comment(lib,TOADLET_LIBRESID_NAME)
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

class SIDAttributes{
public:
	SIDAttributes():
		tune(0){}

	#if SIDPLAY_VERSION==1
		emuEngine player;
		emuConfig config;
		sidTune tune;
	#elif SIDPLAY_VERSION==2
		sidplay2 player;
		sid2_config_t config;
		sid2_info_t info;
		SidTune tune;
	#endif
};

SIDDecoder::SIDDecoder():
	sid(NULL)
{
	sid=new SIDAttributes();
}

SIDDecoder::~SIDDecoder(){
	delete sid;
}

bool SIDDecoder::startStream(Stream::ptr stream){
	bool result=false;
	tbyte *tuneBuffer=NULL;
	int tuneBufferLength=0;
	int song=0;

	AudioFormatConversion::decode(stream,tuneBuffer,tuneBufferLength);
#if SIDPLAY_VERSION==1
	result=sid->tune.load(tuneBuffer,tuneBufferLength);
#elif SIDPLAY_VERSION==2
	result=sid->tune.read(tuneBuffer,tuneBufferLength);
#endif
	delete[] tuneBuffer;
	if(result==false){
		return false;
	}

#if SIDPLAY_VERSION==1
	sid->player.getConfig(mConfig);

	sid->config.sampleFormat=SIDEMU_UNSIGNED_PCM;
	sid->config.bitsPerSample=16;
	sid->config.channels=1;

	sid->player.setConfig(sid->config);

	result=sidEmuInitializeSong(sid->player,sid->tune,song);
	if(result==false){
		Error::unknown(Categories::TOADLET_RIBBIT,"Error initializing song");
		return false;
	}
#elif SIDPLAY_VERSION==2
	sid->config=sid->player.config();
	sid->info=sid->player.info();

	ReSIDBuilder *resid=new ReSIDBuilder("ReSID");
	resid->create(sid->info.maxsids);
	resid->sampling(getSamplesPerSecond());
	sid->config.sidEmulation=resid;
	sid->config.frequency=getSamplesPerSecond();
	sid->config.precision=getBitsPerSample();
	sid->config.playback=(getChannels()==2?sid2_stereo:sid2_mono);

	sid->player.config(sid->config);

	sid->tune.selectSong(song);

	result=(sid->player.load(&sid->tune)>=0);
	if(result==false){
		Error::unknown(Categories::TOADLET_RIBBIT,sid->player.error());
		return false;
	}

	sid->player.fastForward(100);
#endif

	return result;
}

#if SIDPLAY_VERSION==1
	int SIDDecoder::getBitsPerSample(){return sid->config.bitsPerSample;}
	int SIDDecoder::getChannels(){return sid->config.channels;}
	int SIDDecoder::getSamplesPerSecond(){return sid->config.frequency;}
	int SIDDecoder::read(tbyte *buffer,int length){sidEmuFillBuffer(sid->player,sid->tune,buffer,length);return length;}
#elif SIDPLAY_VERSION==2
	int SIDDecoder::getBitsPerSample(){return sid->config.precision;}
	int SIDDecoder::getChannels(){return sid->info.channels;}
	int SIDDecoder::getSamplesPerSecond(){return sid->config.frequency;}
	int SIDDecoder::read(tbyte *buffer,int length){return sid->player.play(buffer,length);}
#endif

}
}
