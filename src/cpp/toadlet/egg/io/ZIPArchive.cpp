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
#include <toadlet/egg/io/ZIPArchive.h>

namespace toadlet{
namespace egg{
namespace io{

int toadlet_zzip_open(zzip_char_t* name,int flags,...){
	Logger::alert("Open called!");
	return 0;
}

int toadlet_zzip_close(int fd){
	Logger::alert("close called!");
	return 0;
}

zzip_ssize_t toadlet_zzip_read(int fd,void *buf,zzip_size_t len){
	Logger::alert("read called!");
	return 0;
}

zzip_off_t toadlet_zzip_seeks(int fd,zzip_off_t offset,int whence){
	Logger::alert("seeks called!");
	return 0;
}

zzip_off_t toadlet_zzip_filesize(int fd){
	Logger::alert("filesize called!");
	return 0;
}

zzip_ssize_t toadlet_zzip_write(int fd,_zzip_const void* buf,zzip_size_t len){
	Logger::alert("write called!");
	return 0;
}

ZIPArchive::ZIPArchive(){
	zzip_init_io(&mIO,0);
	mIO.fd.open=toadlet_zzip_open;
	mIO.fd.close=toadlet_zzip_close;
	mIO.fd.read=toadlet_zzip_read;
	mIO.fd.seeks=toadlet_zzip_seeks;
	mIO.fd.filesize=toadlet_zzip_filesize;
	mIO.fd.write=toadlet_zzip_write;
}

ZIPArchive::~ZIPArchive(){
	destroy();
}

void ZIPArchive::destroy(){
	if(mStream!=NULL){
		mStream->close();
		mStream=NULL;
	}

	zzip_close(NULL,0,0,NULL,&mIO);
}

bool ZIPArchive::open(Stream::ptr stream){
	mStream=stream;

	zzip_open_ext_io(NULL,0,0,NULL,&mIO);

	mDataOffset=0;

	byte signature[4];
	mDataOffset+=mStream->read(signature,4);
	if(signature[0]!='T' || signature[1]!='P' || signature[2]!='K' || signature[3]!='G'){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of ZIP format");
		mStream=NULL;
		return false;
	}

	uint32 version=0;
	mDataOffset+=mStream->readBigUInt32(version);
	if(version!=1){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not ZIP version 1");
		mStream=NULL;
		return false;
	}

	int32 numFiles=0;
	mDataOffset+=mStream->readBigInt32(numFiles);
	int i;
	for(i=0;i<numFiles;++i){
		uint32 nameLength=0;
		mDataOffset+=mStream->readBigUInt32(nameLength);
		byte *name=new byte[nameLength+1];
		mDataOffset+=mStream->read(name,nameLength);
		name[nameLength]=0;
		
		Index index;
		mDataOffset+=mStream->readBigUInt32(index.position);
		mDataOffset+=mStream->readBigUInt32(index.length);

		mIndex[name]=index;
		delete[] name;
	}

	return true;
}

Stream::ptr ZIPArchive::openStream(const String &name){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Creating InputStream for "+name);

	if(mStream==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"ZIP not opened");
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
		byte *data=mMemoryStream->getCurrentDataPointer();
		return MemoryStream::ptr(new MemoryStream(data,length,length,false));
	}
	else{
		byte *data=new byte[length];
		mStream->read(data,length);
		return MemoryStream::ptr(new MemoryStream(data,length,length,true));
	}
}

}
}
}
