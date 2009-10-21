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

#include "ALAudioBufferPeer.h"
#include "ALPlayer.h"

using namespace toadlet::egg;

namespace toadlet{
namespace ribbit{

ALAudioBufferPeer::ALAudioBufferPeer(ALenum format,int sps,char *buffer,int length){
	bufferHandle=0;
	bufferStatic=NULL;

	alGenBuffers(1,&bufferHandle);
	if(toadlet_alBufferDataStatic!=NULL){
		bufferStatic=buffer;
		toadlet_alBufferDataStatic(bufferHandle,format,bufferStatic,length,sps);
	}
	else{
		alBufferData(bufferHandle,format,buffer,length,sps);
	}

	TOADLET_CHECK_ALERROR("ALAudioBufferPeer::ALAudioBufferPeer");
}

ALAudioBufferPeer::~ALAudioBufferPeer(){
	if(bufferHandle!=0){
		alDeleteBuffers(1,&bufferHandle);
		bufferHandle=0;
	}

	if(bufferStatic!=NULL){
		delete[] bufferStatic;
		bufferStatic=NULL;
	}

	// Checking of proper deletion removed to avoid errors when a resource is destroyed after the context is destroyed
	//  There should be no leaks in this case, since the context should clean up any resources it has
	//TOADLET_CHECK_ALERROR("ALAudioBufferPeer::~ALAudioBufferPeer");
}

}
}
