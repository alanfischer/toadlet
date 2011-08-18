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

#pragma warning(disable:4231) // nonstandard extension used : 'extern' before template explicit

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

namespace toadlet{
namespace ribbit{
namespace decoder{

#if SIDPLAY_VERSION==1
	// The sidplay1 engine can not be reused after deallocation, so allocate it statically
	static emuEngine engine;
#endif

class SIDAttributes{
public:
	SIDAttributes():
		tune(0){}

	#if SIDPLAY_VERSION==1
		emuEngine *player;
		emuConfig config;
		sidTune *tune;
	#elif SIDPLAY_VERSION==2
		sidplay2 *player;
		sid2_config_t config;
		sid2_info_t info;
		SidTune *tune;
	#endif
};

SIDDecoder::SIDDecoder():
	sid(NULL)
{
	mFormat=AudioFormat::ptr(new AudioFormat());
	sid=new SIDAttributes();
	#if SIDPLAY_VERSION==1
		sid->player=&engine;
		sid->tune=new sidTune(0);
	#elif SIDPLAY_VERSION==2
		sid->player=new sidplay2();
		sid->tune=new SidTune(0);
	#endif
}

SIDDecoder::~SIDDecoder(){
	delete sid->player;
	delete sid->tune;
	delete sid;
}

bool SIDDecoder::startStream(Stream::ptr stream){
	bool result=false;
	tbyte *tuneBuffer=NULL;
	int tuneBufferLength=0;
	int song=0;

	tbyte type[4];
	int amount=stream->read(type,4);
	stream->reset();

	if(amount<4 || (memcmp(type,"PSID",4)!=0 && memcmp(type,"RSID",4)!=0)){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"not PSID or RSID type");
		return false;
	}

	AudioFormatConversion::decode(stream,tuneBuffer,tuneBufferLength);
#if SIDPLAY_VERSION==1
	result=sid->tune->load(tuneBuffer,tuneBufferLength);
#elif SIDPLAY_VERSION==2
	result=sid->tune->read(tuneBuffer,tuneBufferLength);
#endif
	delete[] tuneBuffer;
	if(result==false){
		return false;
	}

#if SIDPLAY_VERSION==1
	sid->player->getConfig(mConfig);
	sid->config.sampleFormat=SIDEMU_UNSIGNED_PCM;
	sid->config.bitsPerSample=16;
	sid->config.channels=1;

	mFormat->bitsPerSample=sid->config.bitsPerSample;
	mFormat->channels=sid->config.channels;
	mFormat->samplesPerSecond=sid->config.frequency;

	sid->player->setConfig(sid->config);

	result=sidEmuInitializeSong(sid->player,sid->tune,song);
	if(result==false){
		Error::unknown(Categories::TOADLET_RIBBIT,"Error initializing song");
		return false;
	}
#elif SIDPLAY_VERSION==2
	sid->config=sid->player->config();
	sid->info=sid->player->info();

	mFormat->bitsPerSample=sid->config.precision;
	mFormat->channels=sid->info.channels;
	mFormat->samplesPerSecond=sid->config.frequency;

	ReSIDBuilder *resid=new ReSIDBuilder("ReSID");
	resid->create(sid->info.maxsids);
	resid->sampling(sid->config.frequency);
	sid->config.sidEmulation=resid;
	sid->config.playback=(sid->info.channels==2?sid2_stereo:sid2_mono);

	sid->player->config(sid->config);

	sid->tune->selectSong(song);

	result=(sid->player->load(sid->tune)>=0);
	if(result==false){
		Error::unknown(Categories::TOADLET_RIBBIT,sid->player->error());
		return false;
	}

	sid->player->fastForward(100);
#endif

	return result;
}

int SIDDecoder::read(tbyte *buffer,int length){
#if SIDPLAY_VERSION==1
	sidEmuFillBuffer(&sid->player,&sid->tune,buffer,length);
#elif SIDPLAY_VERSION==2
	length=sid->player->play(buffer,length);
#endif
	return length;
}

}
}
}
