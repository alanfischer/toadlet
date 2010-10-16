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
	mPlaying(false),
	mLooping(false),
	mGain(Math::ONE),
	mPitch(Math::ONE),

	mPosition(0)
{
	mPlayer=player;
}

Win32Audio::~Win32Audio(){
	destroy();
}

bool Win32Audio::create(AudioBuffer::ptr audioBuffer){
	destroy();

	// For now we just make sure things are in the correct format, in the future we will convert them
	Win32AudioBuffer::ptr win32AudioBuffer=shared_static_cast<Win32AudioBuffer>(audioBuffer);
	TOADLET_ASSERT(win32AudioBuffer->mChannels==1);
	TOADLET_ASSERT(win32AudioBuffer->mBitsPerSample==8);

	mPlayer->internal_audioCreate(this);
	mAudioBuffer=win32AudioBuffer;
	return true;
}

bool Win32Audio::create(Stream::ptr stream,const String &mimeType){
	destroy();

	return false;
}

void Win32Audio::destroy(){
	mPlayer->internal_audioDestroy(this);
}

int Win32Audio::read(int8 *data,int length){
	int byteAmount=Math::minVal(length*1 /* channels * bps */,mAudioBuffer->mLength-mPosition);

	memcpy(data,mAudioBuffer->mData,byteAmount);

	mPosition+=byteAmount;

	return byteAmount/1;
}

}
}
