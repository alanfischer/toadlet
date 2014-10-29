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

#include "TPKGArchive.h"

namespace toadlet{
namespace tadpole{

TPKGArchive::TPKGArchive():
	//mIndex,
	mDataOffset(0)
	//mStream,
	//mMemoryStream
{}

void TPKGArchive::destroy(){
	if(mStream!=NULL){
		mStream->close();
		mStream=NULL;
	}

	mMemoryStream=NULL;
}

bool TPKGArchive::open(MemoryStream::ptr memoryStream){
	mMemoryStream=memoryStream;
	return open((Stream::ptr)memoryStream);
}

bool TPKGArchive::open(Stream::ptr stream){
	mStream=new DataStream(stream);

	mDataOffset=0;

	tbyte signature[4];
	mDataOffset+=mStream->read(signature,4);
	if(signature[0]!='T' || signature[1]!='P' || signature[2]!='K' || signature[3]!='G'){
		mStream=NULL;
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of TPKG format");
		return false;
	}

	uint32 version=mStream->readBUInt32();mDataOffset+=4;
	if(version!=1){
		mStream=NULL;
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not TPKG version 1");
		return false;
	}

	int32 numFiles=mStream->readBInt32();mDataOffset+=4;
	int i;
	for(i=0;i<numFiles;++i){
		uint32 nameLength=mStream->readBUInt32();mDataOffset+=4;
		tbyte *name=new tbyte[nameLength+1];
		mDataOffset+=mStream->read(name,nameLength);
		name[nameLength]=0;
		
		Index index;
		index.position=mStream->readBUInt32();mDataOffset+=4;
		index.length=mStream->readBUInt32();mDataOffset+=4;

		mEntries.push_back(name);
		mIndex[name]=index;
		delete[] name;
	}

	return true;
}

Stream::ptr TPKGArchive::openStream(const String &name){
	Log::debug(Categories::TOADLET_TADPOLE,
		"creating InputStream for "+name);

	if(mStream==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,"null stream");
		return NULL;
	}

	Map<String,Index>::iterator it;
	it=mIndex.find(name);
	if(it==mIndex.end()){
		//Error::fileNotFound(Categories::TOADLET_TADPOLE,"unable to find file");
		return NULL;
	}

	mStream->seek(mDataOffset+it->second.position);
	int length=it->second.length;

	Stream::ptr stream;
	if(mMemoryStream!=NULL){
		tbyte *data=mMemoryStream->getCurrentDataPointer();
		stream=new MemoryStream(data,length,length,false);
	}
	else{
		tbyte *data=new tbyte[length];
		mStream->read(data,length);
		stream=new MemoryStream(data,length,length,true);
	}

	return stream;
}

}
}
