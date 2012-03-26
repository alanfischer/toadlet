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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include "TPKGArchive.h"

namespace toadlet{
namespace tadpole{

TPKGArchive::TPKGArchive():
	//mIndex,
	mDataOffset(0)
	//mStream,
	//mMemoryStream
{}

TPKGArchive::~TPKGArchive(){
	destroy();
}

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
	mStream=DataStream::ptr(new DataStream(stream));

	mDataOffset=0;

	tbyte signature[4];
	mDataOffset+=mStream->read(signature,4);
	if(signature[0]!='T' || signature[1]!='P' || signature[2]!='K' || signature[3]!='G'){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of TPKG format");
		mStream=NULL;
		return false;
	}

	uint32 version=mStream->readBUInt32();mDataOffset+=4;
	if(version!=1){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not TPKG version 1");
		mStream=NULL;
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

		mIndex[name]=index;
		delete[] name;
	}

	return true;
}

Stream::ptr TPKGArchive::openStream(const String &name){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Creating InputStream for "+name);

	if(mStream==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"TPKG not opened");
		return NULL;
	}

	Map<String,Index>::iterator it;
	it=mIndex.find(name);
	if(it==mIndex.end()){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"File not found in data file");
		return NULL;
	}

	mStream->seek(mDataOffset+it->second.position);
	int length=it->second.length;

	if(mMemoryStream!=NULL){
		tbyte *data=mMemoryStream->getCurrentDataPointer();
		return MemoryStream::ptr(new MemoryStream(data,length,length,false));
	}
	else{
		tbyte *data=new tbyte[length];
		mStream->read(data,length);
		return MemoryStream::ptr(new MemoryStream(data,length,length,true));
	}
}

Collection<String>::ptr TPKGArchive::getEntries(){
	Collection<String>::ptr entries(new Collection<String>());
	for(Map<String,Index>::iterator it=mIndex.begin();it!=mIndex.end();++it){
		entries->add(it->first);
	}
	return entries;
}

}
}
