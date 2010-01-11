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

#include "Win32AudioBuffer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

Win32AudioBuffer::Win32AudioBuffer(Win32Player *player):BaseResource(),
	mAudioPlayer(NULL),
	//mWaveFormat(0),
	//mWaveHDR(0),
	mTime(0)
{
	memset(&mWaveFormat,0,sizeof(mWaveFormat));
	memset(&mWaveHDR,0,sizeof(mWaveHDR));

	mAudioPlayer=player;
}

Win32AudioBuffer::~Win32AudioBuffer(){
	destroy();
}

bool Win32AudioBuffer::create(Stream::ptr stream,const String &mimeType){
	AudioStream::ptr decoder=mAudioPlayer->startAudioStream(stream,mimeType);
	if(decoder==NULL){
		return false;
	}

	char *buffer=0;
	int length=0;
	int channels=decoder->getChannels();
	int sps=decoder->getSamplesPerSecond();
	int bps=decoder->getBitsPerSample();

	mAudioPlayer->decodeStream(decoder,buffer,length);

	// WaveFormat
	mWaveFormat.wFormatTag=WAVE_FORMAT_PCM;
	mWaveFormat.nChannels=channels;
	mWaveFormat.nSamplesPerSec=sps;
    mWaveFormat.wBitsPerSample=bps;
	mWaveFormat.nBlockAlign=channels*bps/8;
	mWaveFormat.nAvgBytesPerSec=sps*mWaveFormat.nBlockAlign;
	// WaveHDR
	mWaveHDR.lpData=buffer;
	mWaveHDR.dwBufferLength=length;
	// Time
	mTime=length*1000*8/(channels*bps*sps);

	HRESULT hr=0;
	if(mAudioPlayer->getWaveOut()==NULL){
		HWAVEOUT waveOut=0;
		hr=waveOutOpen(&waveOut,WAVE_MAPPER,&mWaveFormat,NULL,NULL,CALLBACK_NULL);
		if(hr!=MMSYSERR_NOERROR){
			Error::unknown(Categories::TOADLET_RIBBIT,
				"error in waveOutOpen");
			return false;
		}

		mAudioPlayer->setWaveOut(waveOut);
	}

	return prepareHeader();
}

void Win32AudioBuffer::destroy(){
	unprepareHeader();

	if(mWaveHDR.lpData!=NULL){
		delete[] mWaveHDR.lpData;
		mWaveHDR.lpData=NULL;
	}
}

void Win32AudioBuffer::setLoopCount(int loopCount){
	if(loopCount!=mWaveHDR.dwLoops){
		unprepareHeader();

		if(loopCount>0){
			mWaveHDR.dwFlags=WHDR_BEGINLOOP|WHDR_ENDLOOP;
			mWaveHDR.dwLoops=loopCount;
		}
		else{
			mWaveHDR.dwFlags=0;
			mWaveHDR.dwLoops=0;
		}

		prepareHeader();
	}
}

int Win32AudioBuffer::getLoopCount() const{
	return mWaveHDR.dwLoops;
}

bool Win32AudioBuffer::prepareHeader(){
	HRESULT hr=waveOutPrepareHeader(mAudioPlayer->getWaveOut(),&mWaveHDR,sizeof(mWaveHDR));
	if(hr!=MMSYSERR_NOERROR){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"error in waveOutPrepareHeader");
		return false;
	}
	return true;
}

void Win32AudioBuffer::unprepareHeader(){
	waveOutUnprepareHeader(mAudioPlayer->getWaveOut(),&mWaveHDR,sizeof(mWaveHDR));
}

}
}
