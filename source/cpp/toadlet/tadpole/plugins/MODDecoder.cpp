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

#include <toadlet/egg/io/DataStream.h>
#include <toadlet/ribbit/AudioFormatConversion.h>
#include "MODDecoder.h"

namespace toadlet{
namespace tadpole{

MODDecoder::MODDecoder():
	mFile(NULL)
{
	mFormat=new AudioFormat(16,2,22050);
}

MODDecoder::~MODDecoder(){
	if(mFile!=NULL){
		ModPlug_Unload(mFile);
	}
}

bool MODDecoder::openStream(Stream *stream){
	if(mFile!=NULL){
		ModPlug_Unload(mFile);
	}

	ModPlug_Settings settings;
	ModPlug_GetSettings(&settings);
	{
		settings.mChannels=mFormat->getChannels();
		settings.mBits=mFormat->getBitsPerSample();
		settings.mFrequency=mFormat->getSamplesPerSecond();
	}
	ModPlug_SetSettings(&settings);

	int length=stream->length();
	tbyte *data=new tbyte[length];
	stream->read(data,length);
	mFile=ModPlug_Load(data,length);
	delete[] data;

	return mFile!=NULL;
}

int MODDecoder::read(tbyte *buffer,int length){
	return ModPlug_Read(mFile,buffer,length);
}

bool MODDecoder::reset(){
	ModPlug_Seek(mFile,0);
	return true;
}

}
}
