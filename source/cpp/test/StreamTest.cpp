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

#include "StreamTest.h"
#include <toadlet/egg/io/MemoryStream.h>

bool StreamTest::testMessageStream(int messageLength,int maxLength){
	Log::alert(String("Testing messageLength:")+messageLength+" maxLength:"+maxLength);

	MemoryStream::ptr memoryStream=new MemoryStream();
	int funkyMod=53;

	PacketMessageStream::ptr outStream=new PacketMessageStream(memoryStream,128,maxLength);
	PacketMessageStream::ptr inStream=new PacketMessageStream(memoryStream,128,maxLength);
	
	int id=12;
	outStream->writeMessage(id);
	for(int i=0;i<messageLength;++i){
		tbyte b=i%funkyMod;
		if(i==maxLength){
			int y=0;
		}
		outStream->write(&b,1);
	}
	outStream->flush();

	memoryStream->seek(0);

	int readId=0;
	while((readId=inStream->readMessage())==0);
	if(id!=readId){
		Log::alert("incorrect message id");
		return false;
	}

	for(int i=0;i<messageLength;++i){
		tbyte b=0;
		inStream->read(&b,1);
		if(b!=i%funkyMod){
			Log::alert("incorrect message data");
			return false;
		}
	}
	inStream->reset();

	return true;
}

bool StreamTest::run(){
	bool result=true;

	Log::alert("Testing Streams");
	result&=testMessageStream(128,1024);
	result&=testMessageStream(2048,1024);
	Log::alert(result?"Succeeded":"Failed");

	return result;
}
