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

#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/handler/WaveDecoder.h>

namespace toadlet{
namespace tadpole{
namespace handler{

#if !defined(WAVE_Format_PCM)
#	define WAVE_Format_PCM 0x0001
#endif
#if !defined(WAVE_Format_IMA_ADPCM)
#	define WAVE_Format_IMA_ADPCM 0x0011
#endif

typedef struct {
	unsigned short	wFormatTag;
	unsigned short	nChannels;
	unsigned int	nSamplesPerSec;
	unsigned int	nAvgBytesPerSec;
	unsigned short	nBlockAlign;
	unsigned short	wBitsPerSample;
} WAVEFORMAT;

typedef struct {
	unsigned int	fccID;
	int				dwSize;
	unsigned int	fccType;
} RIFFHEADER;

typedef struct {
	unsigned int	fccTag;
	int				dwSize;
} CHUNKHEADER;

WaveDecoder::WaveDecoder():
	mData(NULL),
	mSize(0),
	mPosition(0)
{
	mFormat=AudioFormat::ptr(new AudioFormat());
}

WaveDecoder::~WaveDecoder(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

bool WaveDecoder::startStream(Stream::ptr stream){
	RIFFHEADER header;
	CHUNKHEADER chunk;
	WAVEFORMAT fmt;

	mStream=stream;
	mFormat->channels=1;
	mFormat->bitsPerSample=16;
	mFormat->samplesPerSecond=22050;

	if(stream->read((tbyte*)&header,sizeof(header))!=sizeof(header)){
		Logger::error("unable to read header");
		return false;
	}

	// No need to swap strings
	//littleUInt32InPlace(header.fccID);
	littleInt32InPlace(header.dwSize);
	// No need to swap strings
	//littleUInt32InPlace(header.fccType);

	if(memcmp(&header.fccID,"RIFF",4)!=0 || memcmp(&header.fccType,"WAVE",4)!=0){
		Logger::error("not RIFF WAVE format");
		return false;
	}

	header.dwSize += (header.dwSize & 1) - sizeof(header.fccType);

	while(header.dwSize>0){
		if(stream->read((tbyte*)&chunk,sizeof(chunk))!=sizeof(chunk)){
			break;
		}

		// No need to swap strings
		//littleUInt32InPlace(chunk.fccTag);
		littleInt32InPlace(chunk.dwSize);

		chunk.dwSize+=chunk.dwSize & 1;

		if(memcmp(&chunk.fccTag,"fmt ",4)==0){
			if(stream->read((tbyte*)&fmt,sizeof(fmt))!=sizeof(fmt)){
				break;
			}

			littleUInt16InPlace(fmt.wFormatTag);
			littleUInt16InPlace(fmt.nChannels);
			littleUInt32InPlace(fmt.nSamplesPerSec);
			littleUInt32InPlace(fmt.nAvgBytesPerSec);
			littleUInt16InPlace(fmt.nBlockAlign);
			littleUInt16InPlace(fmt.wBitsPerSample);

			mFormat->channels=fmt.nChannels;
			mFormat->bitsPerSample=fmt.wBitsPerSample;
			mFormat->samplesPerSecond=fmt.nSamplesPerSec;

			skip(stream,chunk.dwSize-sizeof(fmt));
		}
		else if(memcmp(&chunk.fccTag,"data",4)==0){
			if(fmt.wFormatTag==WAVE_Format_PCM){
				mSize=chunk.dwSize;
				mData=new tbyte[chunk.dwSize];
				if(stream->read(mData,chunk.dwSize)!=chunk.dwSize){
					break;
				}
			}
			else if(fmt.wFormatTag==WAVE_Format_IMA_ADPCM){
				mSize=4*chunk.dwSize;
				mData=new tbyte[4*chunk.dwSize];
				if(stream->read(mData+3*chunk.dwSize,chunk.dwSize)!=chunk.dwSize){
					break;
				}
				ADPCMDecoder((char*)mData+3*chunk.dwSize,(short*)mData,chunk.dwSize);
			}
			else{
				skip(stream,chunk.dwSize);
			}
		}
		else{
			skip(stream,chunk.dwSize);
		}

		header.dwSize-=chunk.dwSize+sizeof(chunk);
	}

	return true;
}

int WaveDecoder::read(tbyte *buffer,int length){
	if(mSize-mPosition<length){
		length=mSize-mPosition;
	}

	memcpy(buffer,mData+mPosition,length);

	mPosition+=length;

	return length;
}

bool WaveDecoder::reset(){
	mPosition=0;
	return true;
}

void WaveDecoder::skip(Stream::ptr stream,int amount){
	tbyte *skipBuffer=new tbyte[amount];
	stream->read(skipBuffer,amount);
	delete[] skipBuffer;
}

void WaveDecoder::ADPCMDecoder(const char *in,short *out,int len){
	/* Intel ADPCM step variation table */
	static const int kIndexTable[16]={
	    -1, -1, -1, -1, 2, 4, 6, 8,
	    -1, -1, -1, -1, 2, 4, 6, 8,
	};
	
	static const int kStepSizeTable[89]={
	    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
	    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
	};

	int predictor, index, nibble;

	predictor = 0;
	index = 0;

	while (len > 0) {
		int value = *in++;

		for (nibble = 0; nibble < 2; nibble++) {
			int delta, step, diff;
			
			delta = value & 0x0f;
			value >>= 4;
			
			step = kStepSizeTable[index];
	
			index += kIndexTable[delta];
			if (index < 0)
				index = 0;
			else if (index > 88)
				index = 88;
			
			diff = step >> 3;
			if ((delta & 4) != 0)
				diff += step;
			if ((delta & 2) != 0)
				diff += step >> 1;
			if ((delta & 1) != 0)
				diff += step >> 2;
			
			if ((delta & 8) != 0)
				predictor -= diff;
			else
				predictor += diff;
			
			if (predictor > 32767)
				predictor = 32767;
			else if (predictor < -32768)
				predictor = -32768;
			
			*out++ = predictor;
		}

		len--;
	}
}

}
}
}
