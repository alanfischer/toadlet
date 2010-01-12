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

#ifndef TOADLET_RIBBIT_WAVEDECODER_H
#define TOADLET_RIBBIT_WAVEDECODER_H

#include <toadlet/ribbit/AudioStream.h>

namespace toadlet{
namespace ribbit{

class WaveDecoder:public AudioStream{
public:
	WaveDecoder();
	virtual ~WaveDecoder();

	void close(){}

	bool isReadable(){return true;}
	int read(byte *buffer,int length);

	bool isWriteable(){return false;}
	int write(const byte *buffer,int length){return 0;}

	bool startStream(egg::io::Stream::ptr stream);
	bool stopStream();
	bool reset();
	int length(){return 0;}
	int position(){return 0;}
	bool seek(int offs){return false;}

	int getChannels(){return mChannels;}
	int getSamplesPerSecond(){return mSamplesPerSecond;}
	int getBitsPerSample(){return mBitsPerSample;}
	egg::io::Stream::ptr getParentStream(){return mStream;}

private:
	void skip(egg::io::Stream::ptr stream,int amount);
	void ADPCMDecoder(const char *in,short *out,int len);

	int mChannels;
	int mSamplesPerSecond;
	int mBitsPerSample;
	byte *mData;
	int mSize;
	int mPosition;
	egg::io::Stream::ptr mStream;
};

}
}

#endif

