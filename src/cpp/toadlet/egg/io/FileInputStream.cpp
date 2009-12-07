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

#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace egg{
namespace io{

FileInputStream::FileInputStream(const String &filename){
	Logger::excess(Categories::TOADLET_EGG,"Opening file: "+filename);

	mFile=fopen(filename,"rb");
	mAutoClose=true;

	if(mFile!=NULL){
		fseek(mFile,0,SEEK_END);
		mLength=ftell(mFile);
		fseek(mFile,0,SEEK_SET);
	}
	else{
		mLength=0;
	}
}

FileInputStream::~FileInputStream(){
	if(mFile && mAutoClose){
		fclose(mFile);
		mFile=NULL;
	}
}

bool FileInputStream::isOpen() const{
	return mFile!=NULL;
}

int FileInputStream::read(char *buffer,int length){
	return fread(buffer,1,length,mFile);
}

bool FileInputStream::reset(){
	if(mFile){
		fseek(mFile,0,SEEK_SET);
	}

	return true;
}

bool FileInputStream::seek(int offs){
	if(mFile){
		fseek(mFile,offs,SEEK_SET);
	}

	return true;
}

int FileInputStream::available(){
	if(mFile){
		return mLength-ftell(mFile);
	}
	else{
		return 0;
	}
}

void FileInputStream::close(){
	if(mFile && mAutoClose){
		fclose(mFile);
		mFile=NULL;
	}
}

void FileInputStream::setFILE(FILE *file){
	mFile=file;
}

FILE *FileInputStream::getFILE(){
	return mFile;
}

}	
}
}

