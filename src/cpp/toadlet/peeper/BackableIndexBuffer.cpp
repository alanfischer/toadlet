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

#include <toadlet/peeper/BackableIndexBuffer.h>
#include <toadlet/egg/Error.h>
#include <string.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

BackableIndexBuffer::BackableIndexBuffer():
	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mDataSize(0),
	mIndexFormat(IndexFormat_UINT_8),
	mSize(0),
	
	mData(NULL)
	//mBack
{
}

BackableIndexBuffer::~BackableIndexBuffer(){
	destroy();
}

bool BackableIndexBuffer::create(int usage,int access,IndexFormat indexFormat,int size){
	mUsage=usage;
	mAccess=access;
	mIndexFormat=indexFormat;
	mSize=size;
	mDataSize=mIndexFormat*mSize;

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	mData=new uint8[mDataSize];

	return true;
}

void BackableIndexBuffer::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	if(mListener!=NULL){
		mListener->bufferDestroyed(this);
	}
}

uint8 *BackableIndexBuffer::lock(int lockAccess){
	if(mBack!=NULL){
		return mBack->lock(lockAccess);
	}
	else{
		return mData;
	}
}

bool BackableIndexBuffer::unlock(){
	if(mBack!=NULL){
		return mBack->unlock();
	}
	else{
		return true;
	}
}

void BackableIndexBuffer::setBack(IndexBuffer::ptr back){
	if(back!=mBack && mBack!=NULL){
		mData=new uint8[mDataSize];
		TOADLET_TRY
			byte *data=lock(Access_BIT_READ);
			if(data!=NULL){
				memcpy(mData,data,mDataSize);
				mBack->unlock();
			}
		TOADLET_CATCH(const Exception &){}
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL && mData!=NULL){
		// Create texture on setting the back, otherwise D3D10 static textures will not load data in load
		mBack->create(mUsage,mAccess,mIndexFormat,mSize);

		TOADLET_TRY
			byte *data=mBack->lock(Access_BIT_WRITE);
			if(data!=NULL){
				memcpy(data,mData,mDataSize);
				mBack->unlock();
			}
		TOADLET_CATCH(const Exception &){}
		delete[] mData;
		mData=NULL;
	}
}

}
}
