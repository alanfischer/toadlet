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

#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace egg{
namespace io{

FileOutputStream::FileOutputStream(const String &filename){
	Logger::excess(Categories::TOADLET_EGG,"Opening file: "+filename);

	mFile=fopen(filename,"wb");
	mAutoClose=true;
}

FileOutputStream::~FileOutputStream(){
	if(mFile && mAutoClose){
		fclose(mFile);
		mFile=NULL;
	} 
}

void FileOutputStream::close(){
	if(mFile && mAutoClose){
		fclose(mFile);
		mFile=NULL;
	} 
}

bool FileOutputStream::isOpen() const{
	return mFile!=NULL;
}

int FileOutputStream::write(const char *buffer,int length){
	return fwrite(buffer,length,1,mFile);
}

FILE *FileOutputStream::getFILE(){
	return mFile;
}

}	
}
}

