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

#ifndef TOADLET_TADPOLE_WAVEDECODER_H
#define TOADLET_TADPOLE_WAVEDECODER_H

#include <toadlet/ribbit/AudioStream.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API WaveDecoder:public Object,public AudioStream{
public:
	TOADLET_IOBJECT(WaveDecoder);

	WaveDecoder();
	virtual ~WaveDecoder();

	void close(){}
	bool closed(){return false;}

	bool readable(){return true;}
	int read(tbyte *buffer,int length);

	bool writeable(){return false;}
	int write(const tbyte *buffer,int length){return -1;}

	bool startStream(Stream *stream);
	bool flush(){return false;}
	bool reset();
	int length(){return mSize;}
	int position(){return mPosition;}
	bool seek(int offs){return false;}

	AudioFormat::ptr getAudioFormat() const{return mFormat;}

private:
	void skip(Stream *stream,int amount);
	void ADPCMDecoder(const char *in,short *out,int len);

	AudioFormat::ptr mFormat;
	tbyte *mData;
	int mSize;
	int mPosition;
	Stream::ptr mStream;
};

}
}

#endif

