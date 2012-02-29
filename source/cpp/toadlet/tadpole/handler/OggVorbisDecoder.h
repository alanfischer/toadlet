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

#ifndef TOADLET_TADPOLE_HANDLER_OGGVORBISDECODER_H
#define TOADLET_TADPOLE_HANDLER_OGGVORBISDECODER_H

#include <toadlet/ribbit/AudioStream.h>
#include <toadlet/tadpole/Types.h>

struct OggVorbis_File;
struct vorbis_info;

namespace toadlet{
namespace tadpole{
namespace handler{

const int OGGPACKETSIZE=4096;

class TOADLET_API OggVorbisDecoder:public AudioStream{
public:
	TOADLET_SPTR(OggVorbisDecoder);

	OggVorbisDecoder();
	virtual ~OggVorbisDecoder();

	void close();
	bool closed(){return mVorbisInfo==NULL;}

	bool readable(){return true;}
	int read(tbyte *buffer,int length);

	bool writeable(){return false;}
	int write(const tbyte *buffer,int length){return -1;}

	bool startStream(Stream::ptr stream);
	bool stopStream();

	bool reset();
	int length();
	int position();
	bool seek(int offs);

	AudioFormat::ptr getAudioFormat() const{return mFormat;}

private:
	OggVorbis_File *mVorbisFile;
	vorbis_info *mVorbisInfo;
	char mDataBuffer[OGGPACKETSIZE];
	int mDataLength;
	AudioFormat::ptr mFormat;
	Stream::ptr mStream;
};

}
}
}

#endif
