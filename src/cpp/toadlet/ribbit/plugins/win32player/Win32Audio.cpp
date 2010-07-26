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

#include "Win32Audio.h"
#include "Win32AudioBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/MathConversion.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

Win32Audio::Win32Audio(Win32Player *player):
	mPlayer(NULL),
	//mAudioBuffer,
	mPitch(Math::ONE)
{
	mPlayer=player;
}

Win32Audio::~Win32Audio(){
	destroy();
}

bool Win32Audio::create(AudioBuffer::ptr audioBuffer){
	destroy();

	mPlayer->internal_audioCreate(this);
	mAudioBuffer=audioBuffer;
	return true;
}

bool Win32Audio::create(Stream::ptr stream,const String &mimeType){
	destroy();

	return false;
}

void Win32Audio::destroy(){
	mPlayer->internal_audioDestroy(this);
}

bool Win32Audio::play(){
	if(mAudioBuffer==NULL || !mPlayer->canPlaySound()){
		return false;
	}

	Win32AudioBuffer *audioBuffer=((Win32AudioBuffer*)mAudioBuffer->getRootAudioBuffer());

	stop();

	HRESULT hr=waveOutSetPitch(mPlayer->getWaveOut(),MathConversion::scalarToFixed(mPitch));

	hr=waveOutWrite(mPlayer->getWaveOut(),audioBuffer->getLPWAVEHDR(),sizeof(WAVEHDR)); 
	if(hr!=MMSYSERR_NOERROR){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"error in waveOutWrite");
		return false;
	}

	// We'll just use a set time of 400 ms between allowed sounds, otherwise it gets too quiet
	//  Alternatly, we could use audioBuffer->time, which would be the whole sound time
	mPlayer->playedSound(400);

	return true;
}

bool Win32Audio::stop(){
	waveOutReset(mPlayer->getWaveOut());

	return true;
}

void Win32Audio::setGain(scalar gain){
	if(gain<0) gain=0;
	if(gain>Math::ONE) gain=Math::ONE;
	DWORD volume=0xFFFF*gain / Math::ONE;
	waveOutSetVolume(mPlayer->getWaveOut(),(volume<<16) | volume);
}

void Win32Audio::setLooping(bool looping){
	Win32AudioBuffer *audioBuffer=((Win32AudioBuffer*)mAudioBuffer->getRootAudioBuffer());
	audioBuffer->setLoopCount(looping?Extents::MAX_INT:0);
}

bool Win32Audio::getLooping() const{
	Win32AudioBuffer *audioBuffer=((Win32AudioBuffer*)mAudioBuffer->getRootAudioBuffer());
	return audioBuffer->getLoopCount()>0;;
}

}
}
