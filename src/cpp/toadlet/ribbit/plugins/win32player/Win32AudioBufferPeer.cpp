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

#include "Win32AudioBufferPeer.h"

namespace toadlet{
namespace ribbit{

Win32AudioBufferPeer::Win32AudioBufferPeer(int bps,int channels,int sps,char *buffer,int length,Win32Player *player):
	//waveFormat,
	//waveHDR,
	time(0)
{
	memset(&waveFormat,0,sizeof(waveFormat));
	waveFormat.wFormatTag=WAVE_FORMAT_PCM;
	waveFormat.nChannels=channels;
	waveFormat.nSamplesPerSec=sps;
    waveFormat.wBitsPerSample=bps;
	waveFormat.nBlockAlign=channels*bps/8;
	waveFormat.nAvgBytesPerSec=sps*waveFormat.nBlockAlign;

	memset(&waveHDR,0,sizeof(waveHDR));
	waveHDR.lpData=buffer;
	waveHDR.dwBufferLength=length;

	time=length*1000*8/(channels*bps*sps);
}

Win32AudioBufferPeer::~Win32AudioBufferPeer(){
	if(waveHDR.lpData!=NULL){
		delete[] waveHDR.lpData;
		waveHDR.lpData=NULL;
	}
}

}
}
