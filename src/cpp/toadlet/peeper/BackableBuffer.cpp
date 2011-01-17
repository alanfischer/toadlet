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

#include <toadlet/peeper/BackableBuffer.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

BackableBuffer::BackableBuffer():
	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mDataSize(0),
	mIndexFormat((IndexFormat)0),
	//mVertexFormat,
	mSize(0),

	mHasData(false),
	mData(NULL)
	//mBack
{
}

BackableBuffer::~BackableBuffer(){
	destroy();
}

bool BackableBuffer::create(int usage,int access,IndexFormat indexFormat,int size){
	destroy();

	mUsage=usage;
	mAccess=access;
	mIndexFormat=indexFormat;
	mSize=size;
	mDataSize=mIndexFormat*mSize;

	mHasData=false;
	if(mData==NULL){
		mData=new uint8[mDataSize];
	}

	return true;
}

bool BackableBuffer::create(int usage,int access,VertexFormat::ptr vertexFormat,int size){
	destroy();

	mUsage=usage;
	mAccess=access;
	mVertexFormat=vertexFormat;
	mSize=size;
	mDataSize=mVertexFormat->getVertexSize()*mSize;

	mHasData=false;
	if(mData==NULL){
		mData=new uint8[mDataSize];
	}

	return true;
}

void BackableBuffer::destroy(){
	if(mBack!=NULL){
		if(mIndexFormat!=(IndexFormat)0){
			((IndexBuffer*)(mBack.get()))->destroy();
		}
		else{
			((VertexBuffer*)(mBack.get()))->destroy();
		}
		mBack=NULL;
	}

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	if(mListener!=NULL){
		if(mIndexFormat!=(IndexFormat)0){
			mListener->bufferDestroyed((IndexBuffer*)this);
		}
		else{
			mListener->bufferDestroyed((VertexBuffer*)this);
		}
		mListener=NULL;
	}
}

void BackableBuffer::resetCreate(){
	if(mBack!=NULL){
		if(mIndexFormat!=(IndexFormat)0){
			((IndexBuffer*)(mBack.get()))->resetCreate();
		}
		else{
			((VertexBuffer*)(mBack.get()))->resetCreate();
		}
	}
}

void BackableBuffer::resetDestroy(){
	if(mBack!=NULL){
		if(mIndexFormat!=(IndexFormat)0){
			((IndexBuffer*)(mBack.get()))->resetDestroy();
		}
		else{
			((VertexBuffer*)(mBack.get()))->resetDestroy();
		}
	}
}

uint8 *BackableBuffer::lock(int lockAccess){
	return mData;
}

bool BackableBuffer::unlock(){
	mHasData=true;

	if(mBack!=NULL){
		writeBack();
	}
	else{
		return true;
	}
}

void BackableBuffer::setBack(IndexBuffer::ptr back){
	if(mBack.get()!=back.get() && mBack!=NULL){
		((IndexBuffer*)(mBack.get()))->destroy();
	}

	mBack=back;
	
	if(back!=NULL){
		// Create on setting the back, otherwise D3D10 static resources will not load data in load
		back->create(mUsage,mAccess,mIndexFormat,mSize);

		if(mHasData){
			writeBack();
		}
	}
}

void BackableBuffer::setBack(VertexBuffer::ptr back){
	if(mBack.get()!=back.get() && mBack!=NULL){
		((IndexBuffer*)(mBack.get()))->destroy();
	}

	mBack=back;
	
	if(back!=NULL){
		// Create on setting the back, otherwise D3D10 static resources will not load data in load
		back->create(mUsage,mAccess,mVertexFormat,mSize);

		if(mHasData){
			writeBack();
		}
	}
}

void BackableBuffer::readBack(){
	TOADLET_TRY
		tbyte *data=mBack->lock(Access_BIT_READ);
		if(data!=NULL){
			memcpy(mData,data,mDataSize);
			mBack->unlock();
		}
	TOADLET_CATCH(const Exception &){}
}

void BackableBuffer::writeBack(){
	TOADLET_TRY
		tbyte *data=mBack->lock(Access_BIT_WRITE);
		if(data!=NULL){
			memcpy(data,mData,mDataSize);
			mBack->unlock();
		}
	TOADLET_CATCH(const Exception &){}
}

}
}
