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

#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Log.h>

namespace toadlet{
namespace egg{
namespace io{

FileStream::FileStream(const String &filename,int open):
	mOpen(0),
	mFile(NULL),
	mAutoClose(false)
{
	Log::excess(Categories::TOADLET_EGG,"Opening file: "+filename);

	String mode;
	if((open&Open_BIT_APPEND)!=0){
		mode+="a";
	}
	else if((open&Open_BIT_READ)!=0){
		mode+="r";
	}
	else if((open&Open_BIT_WRITE)!=0){
		mode+="w";
	}

	if((open&Open_BIT_BINARY)!=0){
		mode+="b";
	}

	if((open&Open_BIT_TRUNCATE)!=0){
		mode+="+";
	}

	mOpen=open;
	mFile=fopen(filename,mode);
	mAutoClose=true;
}

FileStream::~FileStream(){
	close();
}

void FileStream::close(){
	if(mFile!=NULL && mAutoClose){
		fclose(mFile);
		mFile=NULL;
	} 
}

bool FileStream::readable(){
	return mFile!=NULL && (mOpen&Open_BIT_READ)!=0;
}

int FileStream::read(tbyte *buffer,int length){
	return fread(buffer,1,length,mFile);
}

bool FileStream::writeable(){
	return mFile!=NULL && (mOpen&Open_BIT_WRITE)!=0;
}

int FileStream::write(const tbyte *buffer,int length){
	return fwrite(buffer,1,length,mFile);
}

bool FileStream::reset(){
	return seek(0);
}

int FileStream::length(){
	int current=ftell(mFile);
	fseek(mFile,0,SEEK_END);
	int length=ftell(mFile);
	fseek(mFile,current,SEEK_SET);
	return length;
}

int FileStream::position(){
	return ftell(mFile);
}

bool FileStream::seek(int offs){
	return fseek(mFile,offs,SEEK_SET)==0;
}

bool FileStream::flush(){
	return fflush(mFile)==0;
}

}	
}
}

