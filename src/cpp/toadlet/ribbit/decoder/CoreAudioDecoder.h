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

#ifndef TOADLET_RIBBIT_DECODER_COREAUDIODECODER_H
#define TOADLET_RIBBIT_DECODER_COREAUDIODECODER_H

#include <toadlet/ribbit/AudioStream.h>
#include <AudioToolbox/AudioToolbox.h>

namespace toadlet{
namespace ribbit{
namespace decoder{

class CoreAudioDecoder:public AudioStream{
public:
	TOADLET_SHARED_POINTERS(CoreAudioDecoder);

	CoreAudioDecoder();
	virtual ~CoreAudioDecoder();

	bool closed(){return mAudioFile==NULL;}
	void close();

	bool readable(){return true;}
	int read(tbyte *buffer,int length);

	bool writeable(){return false;}
	int write(const tbyte *buffer,int length){return -1;}

	bool startStream(Stream::ptr stream);
	bool stopStream();
	bool reset();
	int length(){return -1;}
	int position(){return -1;}
	bool seek(int offs){return false;}

	int getChannels(){return mStreamDescription.mChannelsPerFrame;}
	int getSamplesPerSecond(){return mStreamDescription.mSampleRate;}
	int getBitsPerSample(){return mStreamDescription.mBitsPerChannel;}

	inline AudioFileID getAudioFileID() const{return mAudioFile;}
	inline const AudioStreamBasicDescription &getStreamDescription() const{return mStreamDescription;}
	bool isVariableBitRate() const;

private:
	static OSStatus audioFileRead(void *inRefCon,SInt64 inPosition,UInt32 requestCount,void *buffer,UInt32 *actualCount);
	static SInt64 audioFileGetSize(void *inRefCon);

	int mPosition;
	int mSourceSize;
	int mSourcePosition;
	Stream::ptr mIn;
	AudioFileID mAudioFile;
	AudioStreamBasicDescription mStreamDescription;
};

}
}
}

#endif

