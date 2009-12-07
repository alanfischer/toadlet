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

#include <toadlet/tadpole/TPKGInputStreamFactory.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{

TPKGInputStreamFactory::TPKGInputStreamFactory():
	//mIndex,
	mDataOffset(0)
	//mInputStream,
	//mMemoryInputStream
{}

TPKGInputStreamFactory::~TPKGInputStreamFactory(){
}

bool TPKGInputStreamFactory::init(MemoryInputStream::ptr memoryInputStream){
	mMemoryInputStream=memoryInputStream;
	return init((InputStream::ptr)memoryInputStream);
}

bool TPKGInputStreamFactory::init(InputStream::ptr inputStream){
	mInputStream=DataInputStream::ptr(new DataInputStream(inputStream));

	mDataOffset=0;

	char signature[4];
	mDataOffset+=mInputStream->read(signature,4);
	if(signature[0]!='T' || signature[1]!='P' || signature[2]!='K' || signature[3]!='G'){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of TPKG format");
		mInputStream=NULL;
		return false;
	}

	uint32 version=0;
	mDataOffset+=mInputStream->readBigUInt32(version);
	if(version!=1){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not TPKG version 1");
		mInputStream=NULL;
		return false;
	}

	int32 numFiles=0;
	mDataOffset+=mInputStream->readBigInt32(numFiles);
	int i;
	for(i=0;i<numFiles;++i){
		uint32 nameLength=0;
		mDataOffset+=mInputStream->readBigUInt32(nameLength);
		char *name=new char[nameLength+1];
		mDataOffset+=mInputStream->read(name,nameLength);
		name[nameLength]=0;
		
		Index index;
		mDataOffset+=mInputStream->readBigUInt32(index.position);
		mDataOffset+=mInputStream->readBigUInt32(index.length);

		mIndex[name]=index;
		delete[] name;
	}

	return true;
}

InputStream::ptr TPKGInputStreamFactory::makeInputStream(const String &name){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Creating InputStream for "+name);

	if(mInputStream==NULL){
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

	mInputStream->seek(mDataOffset+it->second.position);
	int length=it->second.length;

	if(mMemoryInputStream!=NULL){
		char *data=mMemoryInputStream->getCurrentDataPointer();
		return MemoryInputStream::ptr(new MemoryInputStream(data,length,false));
	}
	else{
		char *data=new char[length];
		mInputStream->read(data,length);
		return MemoryInputStream::ptr(new MemoryInputStream(data,length,true));
	}
}

}
}

