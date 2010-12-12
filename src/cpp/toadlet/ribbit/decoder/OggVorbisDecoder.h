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

#ifndef TOADLET_RIBBIT_DECODER_OGGVORBISDECODER_H
#define TOADLET_RIBBIT_DECODER_OGGVORBISDECODER_H

#include <toadlet/ribbit/AudioStream.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace toadlet{
namespace ribbit{
namespace decoder{

const int OGGPACKETSIZE=4096;

class TOADLET_API OggVorbisDecoder:public AudioStream{
public:
	OggVorbisDecoder();
	virtual ~OggVorbisDecoder();

	void close();
	bool closed(){return mVorbisInfo==NULL;}

	bool readable(){return true;}
	int read(tbyte *buffer,int length);

	bool writeable(){return false;}
	int write(const tbyte *buffer,int length){return -1;}

	bool startStream(egg::io::Stream::ptr stream);
	bool stopStream();

	bool reset();
	int length();
	int position();
	bool seek(int offs);

	AudioFormat::ptr getAudioFormat(){return mFormat;}

private:
	static size_t read_func(void *ptr,size_t size,size_t nmemb, void *datasource);
	static int seek_func(void *datasource, ogg_int64_t offset, int whence);
	static int close_func(void *datasource);
	static long tell_func(void *datasource);

	OggVorbis_File mVorbisFile;
	vorbis_info *mVorbisInfo;
	char mDataBuffer[OGGPACKETSIZE];
	int mDataLength;
	AudioFormat::ptr mFormat;
	egg::io::Stream::ptr mStream;
};

}
}
}

#endif
