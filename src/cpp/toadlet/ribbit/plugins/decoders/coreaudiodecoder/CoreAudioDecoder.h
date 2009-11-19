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

#ifndef TOADLET_RIBBIT_COREAUDIODECODER_H
#define TOADLET_RIBBIT_COREAUDIODECODER_H

#include <toadlet/ribbit/AudioStream.h>
#include <AudioToolbox/AudioToolbox.h>

#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace ribbit{

class CoreAudioDecoder:public AudioStream{
public:
	TOADLET_SHARED_POINTERS(CoreAudioDecoder);

	CoreAudioDecoder();
	virtual ~CoreAudioDecoder();

	bool startStream(egg::io::InputStream::ptr istream);
	int read(char *buffer,int length);
	bool stopStream();
	bool reset();
	bool seek(int offs);
	int available();
	void close();
	int getChannels();
	int getSamplesPerSecond();
	int getBitsPerSample();
	egg::io::InputStream::ptr getParentStream();

	inline AudioFileID getAudioFileID() const{return mAudioFile;}
	inline const AudioStreamBasicDescription &getStreamDescription() const{return mStreamDescription;}
	bool isVariableBitRate() const;

private:
	void skip(egg::io::InputStream::ptr in,int amount);

	static OSStatus audioFileRead(void *inRefCon,SInt64 inPosition,ByteCount requestCount,void *buffer,ByteCount *actualCount);
	static SInt64 audioFileGetSize(void *inRefCon);

	int mPosition;
	int mSourceSize;
	int mSourcePosition;
	egg::io::InputStream::ptr mIn;
	AudioFileID mAudioFile;
	AudioStreamBasicDescription mStreamDescription;
};

}
}

#endif

