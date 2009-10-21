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

	bool startStream(egg::io::InputStream::ptr istream);
	int read(char *buffer,int length);
	bool stopStream();
	bool reset();
	bool seek(int offs);
	void close();
	int available();
	int getChannels();
	int getSamplesPerSecond();
	int getBitsPerSample();
	egg::io::InputStream::ptr getParentStream();

private:
	static size_t read_func(void *ptr,size_t size,size_t nmemb, void *datasource);
	static int seek_func(void *datasource, ogg_int64_t offset, int whence);
	static int close_func(void *datasource);
	static long tell_func(void *datasource);

	OggVorbis_File mVorbisFile;
	vorbis_info *mVorbisInfo;
	char mDataBuffer[OGGPACKETSIZE];
	int mDataLength;
	egg::io::InputStream::ptr mIn;
};

}
}

#endif
