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

#ifndef TOADLET_RIBBIT_OGGVORBISDECODER_H
#define TOADLET_RIBBIT_OGGVORBISDECODER_H

#include <toadlet/ribbit/AudioStream.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace toadlet{
namespace ribbit{

const int OGGPACKETSIZE=4096;

class OggVorbisDecoder:public AudioStream{
public:
	OggVorbisDecoder();
	virtual ~OggVorbisDecoder();

	void close();
	bool closed(){return mVorbisInfo==NULL;}

	bool readable(){return true;}
	int read(byte *buffer,int length);

	bool writeable(){return false;}
	int write(const byte *buffer,int length){return 0;}

	bool startStream(egg::io::Stream::ptr stream);
	bool stopStream();

	bool reset();
	int length(){return 0;}
	int position(){return 0;}
	bool seek(int offs){return false;}

	int getChannels();
	int getSamplesPerSecond();
	int getBitsPerSample(){return 16;}
	egg::io::Stream::ptr getParentStream(){return mStream;}

private:
	static size_t read_func(void *ptr,size_t size,size_t nmemb, void *datasource);
	static int seek_func(void *datasource, ogg_int64_t offset, int whence);
	static int close_func(void *datasource);
	static long tell_func(void *datasource);

	OggVorbis_File mVorbisFile;
	vorbis_info *mVorbisInfo;
	char mDataBuffer[OGGPACKETSIZE];
	int mDataLength;
	egg::io::Stream::ptr mStream;
};

}
}

#endif
