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

#ifndef TOADLET_RIBBIT_SIDDECODER_H
#define TOADLET_RIBBIT_SIDDECODER_H

#include <toadlet/ribbit/AudioStream.h>

#define SIDPLAY2
#if defined(SIDPLAY1)
	#include <sidplay/player.h>
#else
	#include <sidplay/sidplay2.h>
	#include <sidplay/builders/resid.h>
#endif

namespace toadlet{
namespace ribbit{

/// @todo: Add a cmake check to build this decoder
class SIDDecoder:public AudioStream{
public:
	SIDDecoder();
	virtual ~SIDDecoder();

	bool startStream(Stream::ptr stream);

	int getBitsPerSample();
	int getChannels();
	int getSamplesPerSecond();

	void close(){}
	bool closed(){return false;}
	bool readable(){return true;}
	int read(tbyte *buffer,int length);

	bool writeable(){return false;}
	int write(const tbyte *buffer,int length){return -1;}

	bool reset(){return false;}
	int length(){return -1;}
	int position(){return -1;}
	bool seek(int offs){return false;}

	egg::String mimeType(){return "audio/psid";}

protected:
#if defined(SIDPLAY1)
	emuEngine mPlayer;
	emuConfig mConfig;
	sidTune mTune;
#else
	sidplay2 mPlayer;
	sid2_config_t mConfig;
	sid2_info_t mInfo;
	SidTune mTune;
#endif
};

}
}

#endif
