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
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace egg{
namespace io{

FileStream::FileStream(const String &filename,int openFlags){
	Logger::excess(Categories::TOADLET_EGG,"Opening file: "+filename);

	const char *mode=NULL;
	if((openFlags&OpenFlags_READ)>0 && (openFlags&OpenFlags_BINARY)>0) mode="rb";
	else if((openFlags&OpenFlags_READ)>0) mode="r";
	else if((openFlags&OpenFlags_WRITE)>0 && (openFlags&OpenFlags_BINARY)>0) mode="wb";
	else if((openFlags&OpenFlags_WRITE)>0) mode="w";

	mFile=fopen(filename,mode);
	mAutoClose=true;
}

FileStream::~FileStream(){
	close();
}

bool FileStream::isOpen() const{
	return mFile!=NULL;
}

void FileStream::close(){
	if(mFile && mAutoClose){
		fclose(mFile);
		mFile=NULL;
	} 
}

int FileStream::read(byte *buffer,int length){
	return fread(buffer,length,1,mFile);
}

int FileStream::write(const byte *buffer,int length){
	return fwrite(buffer,length,1,mFile);
}

bool FileStream::reset(){
	return seek(0);
}

int FileStream::length(){
	int current=ftell(mFile);
	int length=fseek(mFile,0,SEEK_END);
	fseek(mFile,0,current);
	return length;
}

int FileStream::position(){
	return ftell(mFile);
}

bool FileStream::seek(int offs){
	return fseek(mFile,offs,SEEK_SET)!=0;
}

}	
}
}

