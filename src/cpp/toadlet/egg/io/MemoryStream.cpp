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

#include <toadlet/egg/io/MemoryStream.h>
#include <string.h> // memcpy

namespace toadlet{
namespace egg{
namespace io{

MemoryStream::MemoryStream(Stream::ptr stream):
	mData(NULL),
	mDataLength(0),
	mInternalData(NULL),
	mLength(0),
	mInitialLength(0),
	mPosition(0),
	mOwnsData(false)
{
	tbyte data[1024];
	int i;
	for(i=stream->read(data,sizeof(data));i>0;i=stream->read(data,sizeof(data))){
		tbyte *newData=new tbyte[mDataLength+i];
		if(mData!=NULL){
			memcpy(newData,mData,mDataLength);
		}
		memcpy(newData+mDataLength,data,i);
		delete[] mData;
		mData=newData;
		mDataLength+=i;
	}
	mOwnsData=true;
	mLength=mDataLength;
	mInitialLength=mDataLength;
}

MemoryStream::MemoryStream(tbyte *data,int dataLength,int initialLength,bool ownsData):
	mData(NULL),
	mDataLength(0),
	mInternalData(NULL),
	mLength(0),
	mInitialLength(0),
	mPosition(0),
	mOwnsData(false)
{
	mData=data;
	mDataLength=dataLength;
	mLength=initialLength;
	mInitialLength=initialLength;
	mOwnsData=ownsData;
}

MemoryStream::MemoryStream():
	mData(NULL),
	mDataLength(0),
	mInternalData(NULL),
	mLength(0),
	mInitialLength(0),
	mPosition(0),
	mOwnsData(false)
{
	mInternalData=new Collection<tbyte>();
}

MemoryStream::~MemoryStream(){
	if(mOwnsData && mData!=NULL){
		delete[] mData;
	}
	mData=NULL;

	if(mInternalData!=NULL){
		delete mInternalData;
		mInternalData=NULL;
	}
}

int MemoryStream::read(tbyte *buffer,int length){
	tbyte *data=(mData!=NULL)?mData:mInternalData->begin();
	int amt=(length<(mLength-mPosition)?length:(mLength-mPosition));
	memcpy(buffer,data+mPosition,amt);
	mPosition+=amt;
	return amt;
}

int MemoryStream::write(const tbyte *buffer,int length){
	if(mData!=NULL){
		length=((mDataLength-mPosition)<length?(mDataLength-mPosition):length);
		memcpy(mData+mPosition,buffer,length);
	}
	else{
		if(length>=mInternalData->capacity()){
			int twiceCapacity=mInternalData->capacity()*2;
			mInternalData->reserve(length>twiceCapacity?length:twiceCapacity);
		}

		int oldsize=mInternalData->size();
		int newsize=oldsize+length;

		mInternalData->resize(newsize);

		tbyte *data=&mInternalData->at(oldsize);
		memcpy(data,buffer,length);
	}
	mPosition+=length;
	mLength=mPosition>mLength?mPosition:mLength;
	return length;
}

bool MemoryStream::reset(){
	mLength=mInitialLength;
	mPosition=0;
	if(mInternalData!=NULL){
		mInternalData->clear();
	}
	return true;
}

bool MemoryStream::seek(int offs){
	if(mData!=NULL){
		mPosition=mLength<offs?mLength:offs;
	}
	else{
		mPosition=mInternalData->size()<offs?mInternalData->size():offs;
	}
	return true;
}

tbyte *MemoryStream::getCurrentDataPointer(){
	tbyte *data=(mData!=NULL)?mData:mInternalData->begin();
	return data+mPosition;
}

tbyte *MemoryStream::getOriginalDataPointer(){
	tbyte *data=(mData!=NULL)?mData:mInternalData->begin();
	return data;
}

}
}
}

