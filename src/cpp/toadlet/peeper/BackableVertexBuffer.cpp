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
#include <toadlet/egg/Error.h>
#include <string.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

BackableVertexBuffer::BackableVertexBuffer():
	mListener(NULL),
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
		TOADLET_TRY
			byte *data=lock(AccessType_READ_ONLY);
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
		mBack->create(mUsageFlags,mAccessType,mVertexFormat,mSize);

		TOADLET_TRY
			byte *data=lock(AccessType_WRITE_ONLY);
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
