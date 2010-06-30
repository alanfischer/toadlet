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
#include <toadlet/egg/io/ZIPStream.h>
#include <zlib.h>
#include <zzip/zzip.h>
#include <zzip/plugin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace toadlet{
namespace egg{
namespace io{

ZIPStream::ZIPStream(Stream::ptr stream,int openFlags):
	mOpenFlags(0),
	//mStream,
	mZStream(NULL),
	mFile(NULL)
{
	if(openFlags==OpenFlags_UNKNOWN){
		openFlags=(stream->readable()?OpenFlags_READ:0) | (stream->writeable()?OpenFlags_WRITE:0);
	}

	if(openFlags==(OpenFlags_READ|OpenFlags_WRITE)){
		Error::invalidParameters("cannot create a ZIPStream that reads and writes");
		return;
	}

	mOpenFlags=openFlags;
	mZStream=malloc(sizeof(z_stream));
	memset(mZStream,0,sizeof(z_stream));

	int result=0;
	if(mOpenFlags&OpenFlags_READ){
	    result=inflateInit((z_streamp)mZStream);
	}
	else if(mOpenFlags&OpenFlags_WRITE){
	    result=deflateInit((z_streamp)mZStream,Z_DEFAULT_COMPRESSION);
	}
	if(result!=Z_OK){
		Error::invalidParameters("error starting z_stream");
		return;
	}

	mStream=stream;
}

ZIPStream::ZIPStream(void *dir,const String &name):
	mOpenFlags(0),
	//mStream,
	mZStream(NULL),
	mFile(NULL)
{
	mFile=zzip_file_open((ZZIP_DIR*)dir,name,O_RDONLY);
}

ZIPStream::~ZIPStream(){
	close();
}

bool ZIPStream::closed(){
	return mStream==NULL?true:(mFile==NULL);
}

void ZIPStream::close(){
	if(mStream!=NULL){
		mStream->close();
		mStream=NULL;
	}

	if(mZStream!=NULL){
		free(mZStream);
		mZStream=NULL;
	}

	if(mFile!=NULL){
		zzip_file_close((ZZIP_FILE*)mFile);
		mFile=NULL;
	}
}

int ZIPStream::read(tbyte *buffer,int length){
	if(mStream!=NULL && (mOpenFlags&OpenFlags_READ)>0){
		/// @todo: Finish zlib stremaing
		return -1;
	}
	else if(mFile=NULL){
		return zzip_file_read((ZZIP_FILE*)mFile,buffer,length);
	}
	return -1;
}

int ZIPStream::write(const tbyte *buffer,int length){
	if(mStream!=NULL && (mOpenFlags&OpenFlags_WRITE)>0){
		/// @todo: Finish zlib stremaing
		return -1;
	}
	else{
		return -1;
	}
}

bool ZIPStream::reset(){
	if(mStream!=NULL){
		return mStream->reset();
	}
	else{
		return zzip_rewind((ZZIP_FILE*)mFile)!=0;
	}
}

int ZIPStream::length(){
	if(mStream!=NULL){
		return mStream->length();
	}
	else{
		int current=zzip_tell((ZZIP_FILE*)mFile);
		zzip_seek((ZZIP_FILE*)mFile,0,SEEK_END);
		int length=zzip_tell((ZZIP_FILE*)mFile);
		zzip_seek((ZZIP_FILE*)mFile,current,SEEK_SET);
		return length;
	}
}

int ZIPStream::position(){
	if(mStream!=NULL){
		return mStream->position();
	}
	else{
		return zzip_tell((ZZIP_FILE*)mFile);
	}
}

bool ZIPStream::seek(int offs){
	if(mStream!=NULL){
		return mStream->seek(offs);
	}
	else{
		return zzip_seek((ZZIP_FILE*)mFile,offs,SEEK_SET)!=0;
	}
}

}
}
}
