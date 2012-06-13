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

#include <toadlet/peeper/VertexBufferAccessor.h>

namespace toadlet{
namespace peeper{

VertexBufferAccessor::VertexBufferAccessor(VertexBuffer *buffer,int access):
	mBuffer(NULL),
	mVertexSize32(0),
	mNativeFixed(false),
	mData(NULL),
	mFixedData(NULL),
	mFloatData(NULL),
	mColorData(NULL)
{
	if(buffer!=NULL){
		lock(buffer,access);
	}
}

VertexBufferAccessor::VertexBufferAccessor(Buffer *buffer,VertexFormat *format,int access):
	mBuffer(NULL),
	mVertexSize32(0),
	mNativeFixed(false),
	mData(NULL),
	mFixedData(NULL),
	mFloatData(NULL),
	mColorData(NULL)
{
	lock(buffer,format,access);
}

VertexBufferAccessor::~VertexBufferAccessor(){
	unlock();
}

void VertexBufferAccessor::lock(Buffer *buffer,VertexFormat *format,int access){
	unlock();

	mBuffer=buffer;
	mVertexSize32=format->getVertexSize()/sizeof(int32);
	int numElements=format->getNumElements();
	TOADLET_ASSERT(numElements>0 && numElements<16);
	for(int i=0;i<numElements;++i){
		mElementOffsets32[i]=format->getElementOffset(i)/sizeof(int32);
	}

	if((format->getElementFormat(0)&VertexFormat::Format_MASK_TYPES)==VertexFormat::Format_TYPE_FIXED_32){
		mNativeFixed=true;
	}
	else{
		mNativeFixed=false;
	}

	mData=mBuffer->lock(access);
	if(mNativeFixed){
		mFixedData=(int32*)mData;
	}
	else{
		mFloatData=(float*)mData;
	}
	if(format->findElement(VertexFormat::Semantic_COLOR)>0){
		mColorData=(uint32*)mData;
	}
}

void VertexBufferAccessor::unlock(){
	if(mBuffer!=NULL){
		mBuffer->unlock();
		mBuffer=NULL;
		mData=NULL;
		mFixedData=NULL;
		mFloatData=NULL;
		mColorData=NULL;
	}
}

}
}
