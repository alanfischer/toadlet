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

#pragma comment(lib,"libsidplay2.lib")
#pragma comment(lib,"resid_builder.lib")

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
	tbyte *tuneBuffer=NULL;
	int tuneBufferLength=0;
	AudioFormatConversion::decode(stream,tuneBuffer,tuneBufferLength);
	bool result=mTune.read(tuneBuffer,tuneBufferLength);
	delete[] tuneBuffer;
	if(result==false){
		return false;
	}

	mTune.selectSong(0);

	sid2_config_t config=mSidplayer.config();

	ReSIDBuilder *resid=new ReSIDBuilder("ReSID");
	resid->create(mSidplayer.info().maxsids);
	resid->sampling(getSamplesPerSecond());
	config.sidEmulation=resid;
	config.frequency=getSamplesPerSecond();
	config.precision=getBitsPerSample();
	config.playback=(getChannels()==2?sid2_stereo:sid2_mono);

	mSidplayer.config(config);

	result=(mSidplayer.load(&mTune)>=0);
	if(result==false){
		Error::unknown(mSidplayer.error());
		return false;
	}

	mSidplayer.fastForward(100);

	return result;
}

}
}
