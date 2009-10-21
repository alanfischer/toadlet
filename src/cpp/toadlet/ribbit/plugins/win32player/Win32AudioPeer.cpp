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

#include "Win32AudioPeer.h"
#include "Win32AudioBufferPeer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/MathConversion.h>

using namespace toadlet::egg;

namespace toadlet{
namespace ribbit{

Win32AudioPeer::Win32AudioPeer(Win32Player *player):
	mPlayer(NULL),
	//mAudioBuffer,
	mPitch(Math::ONE)
{
	mPlayer=player;
}

Win32AudioPeer::~Win32AudioPeer(){
}

bool Win32AudioPeer::loadAudioBuffer(AudioBuffer::ptr audioBuffer){
	Win32AudioBufferPeer *audioBufferPeer=(Win32AudioBufferPeer*)audioBuffer->internal_getAudioBufferPeer();
	if(audioBufferPeer!=NULL){
		mAudioBuffer=audioBuffer;
	}

	return true;
}

bool Win32AudioPeer::loadAudioStream(egg::io::InputStream::ptr in,const egg::String &mimeType){
	return false;
}

bool Win32AudioPeer::play(){
	if(mAudioBuffer==NULL || !mPlayer->canPlaySound()){
		return false;
	}

	stop();

	HRESULT hr=waveOutSetPitch(mPlayer->getWaveOut(),MathConversion::scalarToFixed(mPitch));

	Win32AudioBufferPeer *audioBufferPeer=(Win32AudioBufferPeer*)mAudioBuffer->internal_getAudioBufferPeer();
	hr=waveOutWrite(mPlayer->getWaveOut(),&audioBufferPeer->waveHDR,sizeof(WAVEHDR)); 
	if(hr!=MMSYSERR_NOERROR){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"error in waveOutWrite");
		return false;
	}

	// We'll just use a set time of 400 ms between allowed sounds, otherwise it gets too quiet
	//  Alternatly, we could use audioBufferPeer->time, which would be the whole sound time
	mPlayer->playedSound(400);

	return true;
}

bool Win32AudioPeer::stop(){
	waveOutReset(mPlayer->getWaveOut());

	return true;
}

void Win32AudioPeer::internal_playerDestroy(){
	mPlayer=NULL;
}

}
}
