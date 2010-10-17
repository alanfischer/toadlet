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

#include <toadlet/egg/Logger.h>
#include <toadlet/ribbit/AudioFormatConversion.h>
#include <string.h> //memcpy

using namespace toadlet::egg;

namespace toadlet{
namespace ribbit{

bool AudioFormatConversion::convert(tbyte *src,int srcChannels,int srcBitsPerSample,int srcSamplesPerSecond,tbyte *dst,int dstChannels,int dstBitsPerSample,int dstSamplesPerSecond,int length){
	/// @todo: Add in sps conversion

	int i=0,v=0;
	int sbps=srcBitsPerSample;
	int dbps=dstBitsPerSample;
	int ss=srcChannels*(srcBitsPerSample/8);
	int ds=dstChannels*(dstBitsPerSample/8);
	int numSamples=length/srcChannels/(srcBitsPerSample/8);
	int bpsDiff=srcBitsPerSample-dstBitsPerSample;

	/// @todo: Replace this with individual optimized versions
	for(i=0;i<numSamples;++i){
		if(sbps==8) v=*(uint8*)(src+i*ss);
		else v=*(uint16*)(src+i*ss);
		if(bpsDiff>0) v>>=bpsDiff;
		else if(bpsDiff<0) v<<=(-bpsDiff);
		if(dbps==8) *(uint8*)(dst+i*ds)=v;
		else *(uint16*)(dst+i*ds)=v;

		if(dstChannels==2){
			if(srcChannels==2){
				if(sbps==8) v=*(uint8*)(src+i*ss+ss/2);
				else v=*(uint16*)(src+i*ss+ss/2);
				if(bpsDiff>0) v>>=bpsDiff;
				else if(bpsDiff<0) v<<=(-bpsDiff);
			}
			if(dbps==8) *(uint8*)(dst+i*ds+ds/2)=v;
			else *(uint16*)(dst+i*ds+ds/2)=v;
		}
	}

	return true;
}

}
}

