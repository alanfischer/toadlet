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

#include <toadlet/egg/Extents.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

namespace toadlet{
namespace ribbit{

bool AudioFormatConversion::decode(AudioStream *stream,tbyte *&finalBuffer,int &finalLength){
	DataStream dataStream(stream);

	finalLength=dataStream.readAll(finalBuffer);

	#if !defined(TOADLET_NATIVE_FORMAT)
		if(stream->getAudioFormat()->getBitsPerSample()==16){
			int i=0;
			while(i<finalLength){
				littleInt16InPlace((int16&)finalBuffer[i]);
				i+=2;
			}
		}
	#endif

	return finalLength>0;
}

bool AudioFormatConversion::convert(tbyte *src,int srcLength,AudioFormat *srcFormat,tbyte *dst,int dstLength,AudioFormat *dstFormat){
	int i=0,j=0,v=0;
	int sbps=srcFormat->getBitsPerSample();
	int dbps=dstFormat->getBitsPerSample();
	int sc=srcFormat->getChannels();
	int dc=dstFormat->getChannels();
	int ssps=srcFormat->getSamplesPerSecond();
	int dsps=dstFormat->getSamplesPerSecond();
	int ss=srcFormat->getFrameSize();
	int ds=dstFormat->getFrameSize();
	int numFrames=srcLength/srcFormat->getFrameSize();

	int reqLength=findConvertedLength(srcLength,srcFormat,dstFormat,true);
	if(dstLength<reqLength){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"invalid destination length");
		return false;
	}

	/// @todo: Replace this with individual optimized versions
	/// @todo: While this no longer corrupts memory, it still doesnt upsample properly
	for(i=0;i<numFrames;++i){
		j=(i*(uint64)dsps)/(uint64)ssps;

		if(sbps==8) v=(*(uint8*)(src+i*ss))-128;
		else v=*(int16*)(src+i*ss);
		if(sbps==8 && dbps==16){v=Math::intClamp(Extents::MIN_INT16,Extents::MAX_INT16,v<<8);}
		else if(sbps==16 && dbps==8){v=Math::intClamp(Extents::MIN_INT8,Extents::MAX_INT8,v>>8);}
		if(dbps==8) *(uint8*)(dst+j*ds)=v+128;
		else *(int16*)(dst+j*ds)=v;

		if(dc==2){
			if(sc==2){
				if(sbps==8) v=(*(uint8*)(src+i*ss+ss/2))-128;
				else v=*(int16*)(src+i*ss+ss/2);
				if(sbps==8 && dbps==16){v=Math::intClamp(Extents::MIN_INT16,Extents::MAX_INT16,v<<8);}
				else if(sbps==16 && dbps==8){v=Math::intClamp(Extents::MIN_INT8,Extents::MAX_INT8,v>>8);}
			}
			if(dbps==8) *(uint8*)(dst+j*ds+ds/2)=v+128;
			else *(int16*)(dst+j*ds+ds/2)=v;
		}
	}

	return true;
}

int AudioFormatConversion::findConvertedLength(int length,AudioFormat *srcFormat,AudioFormat *dstFormat,bool roundUp){
	uint64 dsize=dstFormat->getChannels()*dstFormat->getBitsPerSample()*dstFormat->getSamplesPerSecond();
	uint64 ssize=srcFormat->getChannels()*srcFormat->getBitsPerSample()*srcFormat->getSamplesPerSecond();
	length=(length*(uint64)dsize)/(uint64)ssize;
	// Make sure our resulting length is a multiple of dst frameSize
	if(length%dstFormat->getFrameSize()!=0){
		if(roundUp){
			length=((length/dstFormat->getFrameSize())+1)*dstFormat->getFrameSize();
		}
		else{
			length=((length/dstFormat->getFrameSize()))*dstFormat->getFrameSize();
		}
	}
	return length;
}

void AudioFormatConversion::fade(tbyte *buffer,int length,AudioFormat *format,int fadeTime){
	int bps=format->getBitsPerSample();
	int channels=format->getChannels();
	int numFrames=length/format->getFrameSize();
	int ftf=format->getSamplesPerSecond()*fadeTime/1000;
	if(ftf>numFrames){ftf=numFrames;}
	int i,j;
	if(bps==8){
		for(i=0;i<ftf;++i){
			// Fade front
			for(j=0;j<channels;++j){
				buffer[i*channels+j]=(uint8)(((((int)buffer[i*channels+j])-128)*i/ftf)+128);
			}
			// Fade back
			for(j=0;j<channels;++j){
				buffer[(numFrames-i-1)*channels+j]=(uint8)(((((int)buffer[(numFrames-i-1)*channels+j])-128)*i/ftf)+128);
			}
		}
	}
	else if(bps==16){
		int16 *buffer16=(int16*)buffer;
		for(i=0;i<ftf;++i){
			// Fade front
			for(j=0;j<channels;++j){
				buffer16[i*channels+j]=(int16)(((int)buffer16[i*channels+j])*i/ftf);
			}
			// Fade back
			for(j=0;j<channels;++j){
				buffer16[(numFrames-i-1)*channels+j]=(int16)(((int)buffer16[(numFrames-i-1)*channels+j])*i/ftf);
			}
		}
	}
}

}
}

