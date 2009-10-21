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

#include <toadlet/egg/io/MemoryOutputStream.h>
#include <string.h> // memcpy

namespace toadlet{
namespace egg{
namespace io{

MemoryOutputStream::MemoryOutputStream(char *data,int length,bool ownsData){
	mOriginalData=data;
	mData=data;
	mInternalData=NULL;
	mOriginalLength=length;
	mLeft=length;
	mSize=0;

	mOwnsData=ownsData;
}

MemoryOutputStream::MemoryOutputStream(){
	mOriginalData=NULL;
	mData=NULL;
	mInternalData=new Collection<char>();
	mOriginalLength=0;
	mLeft=0;
	mSize=0;

	mOwnsData=false;
}

MemoryOutputStream::~MemoryOutputStream(){
	if(mOwnsData && mOriginalData!=NULL){
		delete[] mOriginalData;
	}
	mOriginalData=NULL;

	mData=NULL;

	if(mInternalData!=NULL){
		delete mInternalData;
		mInternalData=NULL;
	}
}

int MemoryOutputStream::write(const char *buffer,int length){
	if(mData!=NULL){
		int amt=(mLeft<length?mLeft:length);

		memcpy(mData,buffer,amt);
		mData+=amt;
		mLeft-=amt;
		mSize+=length;

		return amt;
	}
	else{
		if(length>=mInternalData->capacity()){
			int twiceCapacity=mInternalData->capacity()*2;
			mInternalData->reserve(length>twiceCapacity?length:twiceCapacity);
		}

		int oldsize=mInternalData->size();
		int newsize=oldsize+length;

		mInternalData->resize(newsize);

		char *data=&mInternalData->at(oldsize);
		memcpy(data,buffer,length);

		mSize+=length;

		return length;
	}
}

void MemoryOutputStream::reset(){
	if(mData!=NULL){
		mData=mOriginalData;
		mLeft=mOriginalLength;
	}
	else{
		mInternalData->clear();
	}
	mSize=0;
}

void MemoryOutputStream::close(){
}

int MemoryOutputStream::getSize(){
	return mSize;
}

char *MemoryOutputStream::getCurrentDataPointer(){
	if(mData!=NULL){
		return mData;
	}
	else{
		return &mInternalData->at(mInternalData->size());
	}
}

char *MemoryOutputStream::getOriginalDataPointer(){
	if(mOriginalData!=NULL){
		return mOriginalData;
	}
	else{
		return &mInternalData->at(0);
	}
}

}
}
}

