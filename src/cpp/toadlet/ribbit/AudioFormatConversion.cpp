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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

namespace toadlet{
namespace ribbit{

bool AudioFormatConversion::decode(Stream *stream,tbyte *&finalBuffer,int &finalLength){
	const static int bufferSize=4096;
	Collection<tbyte*> buffers;
	int amount=0,total=0;
	int i=0;

	finalBuffer=NULL;
	finalLength=0;

	if(stream->length()<=0){
		return false;
	}

	while(true){
		tbyte *buffer=new tbyte[bufferSize];
		if(buffer==NULL){
			int i;
			for(i=0;i<buffers.size();++i){
				delete[] buffers[i];
			}
			return false;
		}

		amount=stream->read(buffer,bufferSize);
		if(amount<=0){
			delete[] buffer;
			break;
		}
		else{
			total+=amount;
			buffers.add(buffer);
		}
	}

	finalBuffer=new tbyte[total];
	finalLength=total;

	for(i=0;i<buffers.size();++i){
		int thing=bufferSize;
		if(total<thing){
			thing=total;
		}
		memcpy(finalBuffer+i*bufferSize,buffers[i],thing);
		total-=bufferSize;
		delete[] buffers[i];
	}

	return true;
}

bool AudioFormatConversion::decode(AudioStream *stream,tbyte *&finalBuffer,int &finalLength){
	bool result=decode((Stream*)stream,finalBuffer,finalLength);

	#if !defined(TOADLET_NATIVE_FORMAT)
		if(stream->getAudioFormat()->bitsPerSample==16){
			int i=0;
			while(i<finalLength){
				littleInt16InPlace((int16&)finalBuffer[i]);
				i+=2;
			}
		}
	#endif

	return result;
}

bool AudioFormatConversion::convert(tbyte *src,int srcLength,AudioFormat *srcFormat,tbyte *dst,int dstLength,AudioFormat *dstFormat){
	int i=0,v=0;
	int sbps=srcFormat->bitsPerSample;
	int dbps=dstFormat->bitsPerSample;
	int sc=srcFormat->channels;
	int dc=dstFormat->channels;
	int ssps=srcFormat->samplesPerSecond;
	int dsps=dstFormat->samplesPerSecond;
	int ss=srcFormat->frameSize();
	int ds=dstFormat->frameSize();
	int numFrames=srcLength/srcFormat->frameSize();

	if(dstLength!=findConvertedLength(srcLength,srcFormat,dstFormat)){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"invalid destination length");
		return false;
	}

	/// @todo: Replace this with individual optimized versions
	/// @todo: While this no longer corrupts memory, it still doesnt upsample properly
	for(i=0;i<numFrames;++i){
		int j=(i*(uint64)dsps)/(uint64)ssps;

		if(sbps==8) v=(*(uint8*)(src+i*ss))-128;
		else v=*(int16*)(src+i*ss);
		if(sbps==8 && dbps==16){v=Math::intClamp(Extents::MIN_INT16,Extents::MAX_INT16,v*256);}
		else if(sbps==16 && dbps==8){v=Math::intClamp(Extents::MIN_INT8,Extents::MAX_INT8,v/256);}
		if(dbps==8) *(uint8*)(dst+j*ds)=v+128;
		else *(int16*)(dst+j*ds)=v;

		if(dc==2){
			if(sc==2){
				if(sbps==8) v=(*(uint8*)(src+i*ss+ss/2))-128;
				else v=*(int16*)(src+i*ss+ss/2);
				if(sbps==8 && dbps==16){v=Math::intClamp(Extents::MIN_INT16,Extents::MAX_INT16,v*256);}
				else if(sbps==16 && dbps==8){v=Math::intClamp(Extents::MIN_INT8,Extents::MAX_INT8,v/256);}
			}
			if(dbps==8) *(uint8*)(dst+j*ds+ds/2)=v+128;
			else *(int16*)(dst+j*ds+ds/2)=v;
		}
	}

	return true;
}

int AudioFormatConversion::findConvertedLength(int length,AudioFormat *srcFormat,AudioFormat *dstFormat){
	uint64 dsize=dstFormat->channels*dstFormat->bitsPerSample*dstFormat->samplesPerSecond;
	uint64 ssize=srcFormat->channels*srcFormat->bitsPerSample*srcFormat->samplesPerSecond;
	length=(length*(uint64)dsize)/(uint64)ssize;
	// Make sure our resulting length is a multiple of dst frameSize
	if(length%dstFormat->frameSize()!=0){
		length=((length/dstFormat->frameSize())+1)*dstFormat->frameSize();
	}
	return length;
}

void AudioFormatConversion::fade(tbyte *buffer,int length,AudioFormat *format,int fadeTime){
	int channels=format->channels;
	int numFrames=length/format->frameSize();
	int ftf=format->samplesPerSecond*fadeTime/1000;
	if(ftf>numFrames){ftf=numFrames;}
	int i,j;
	if(format->bitsPerSample==8){
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
	else if(format->bitsPerSample==16){
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

