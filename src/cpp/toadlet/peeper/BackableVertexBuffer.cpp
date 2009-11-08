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

#include <toadlet/peeper/BackableVertexBuffer.h>
#include <string.h>

namespace toadlet{
namespace peeper{

BackableVertexBuffer::BackableVertexBuffer():
	mUsageFlags(0),
	mAccessType(AccessType_NO_ACCESS),
	mDataSize(0),
	//mVertexFormat,
	mSize(0),
	
	mData(NULL)
	//mBack
{
}

BackableVertexBuffer::~BackableVertexBuffer(){
	destroy();
}

bool BackableVertexBuffer::create(int usageFlags,AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	destroy();

	mUsageFlags=usageFlags;
	mAccessType=accessType;
	mVertexFormat=vertexFormat;
	mSize=size;
	mDataSize=mVertexFormat->getVertexSize()*mSize;
	
	mData=new uint8[mDataSize];

	return true;
}

void BackableVertexBuffer::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
	}

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

uint8 *BackableVertexBuffer::lock(AccessType accessType){
	if(mBack!=NULL){
		return mBack->lock(accessType);
	}
	else{
		return mData;
	}
}

bool BackableVertexBuffer::unlock(){
	if(mBack!=NULL){
		return mBack->unlock();
	}
	else{
		return true;
	}
}

void BackableVertexBuffer::setBack(VertexBuffer::ptr back){
	if(back!=mBack && mBack!=NULL){
		mData=new uint8[mDataSize];
		uint8 *backData=mBack->lock(AccessType_READ_ONLY);
		memcpy(mData,backData,mDataSize);
		mBack->unlock();
	}

	mBack=back;
	
	if(mBack!=NULL && mData!=NULL){
		uint8 *backData=mBack->lock(AccessType_WRITE_ONLY);
		memcpy(backData,mData,mDataSize);
		mBack->unlock();
		delete[] mData;
		mData=NULL;
	}
}

}
}
