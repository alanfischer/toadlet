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

#include <toadlet/egg/io/MemoryInputStream.h>
#include <string.h> // memcpy

namespace toadlet{
namespace egg{
namespace io{

MemoryInputStream::MemoryInputStream(InputStream::ptr in):
	mOriginalData(NULL),
	mData(NULL),
	mOriginalLength(0),
	mLeft(0),
	mOwnsData(false)
{
	char data[1024];
	int i;
	for(i=in->read(data,sizeof(data));i>0;i=in->read(data,sizeof(data))){
		char *newData=new char[mOriginalLength+i];
		if(mOriginalData){
			memcpy(newData,mOriginalData,mOriginalLength);
		}
		memcpy(newData+mOriginalLength,data,i);
		delete[] mOriginalData;
		mOriginalData=newData;
		mOriginalLength+=i;
	}
	mData=mOriginalData;
	mLeft=mOriginalLength;
	mOwnsData=true;
}

MemoryInputStream::MemoryInputStream(char *data,int length,bool ownsData){
	mOriginalData=data;
	mData=data;
	mOriginalLength=length;
	mLeft=length;
	mOwnsData=ownsData;
}

MemoryInputStream::MemoryInputStream(unsigned char *data,int length,bool ownsData){
	mOriginalData=(char*)data;
	mData=(char*)data;
	mOriginalLength=length;
	mLeft=length;
	mOwnsData=ownsData;
}

MemoryInputStream::~MemoryInputStream(){
	if(mOwnsData && mOriginalData!=NULL){
		delete[] mOriginalData;
	}
	mOriginalData=NULL;

	mData=NULL;
}

int MemoryInputStream::read(char *buffer,int length){
	int amt=(length<mLeft?length:mLeft);
	memcpy(buffer,mData,amt);
	mData+=amt;
	mLeft-=amt;
	return amt;
}

bool MemoryInputStream::reset(){
	mData=mOriginalData;
	mLeft=mOriginalLength;
	return true;
}

bool MemoryInputStream::seek(int offs){
	reset();

	int amt=(offs<mOriginalLength?offs:mOriginalLength);
	mData+=amt;
	mLeft=mOriginalLength-amt;

	return true;
}

bool MemoryInputStream::skip(int amt){
	amt=(amt<mLeft?amt:mLeft);
	mData+=amt;
	mLeft-=amt;

	return true;
}

int MemoryInputStream::available(){
	return mLeft;
}

void MemoryInputStream::close(){
}

int MemoryInputStream::getSize(){
	return mOriginalLength;
}

char *MemoryInputStream::getCurrentDataPointer(){
	return mData;
}

char *MemoryInputStream::getOriginalDataPointer(){
	return mOriginalData;
}

}
}
}

