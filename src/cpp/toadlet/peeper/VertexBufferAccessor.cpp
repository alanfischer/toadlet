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
#include <stdlib.h>

namespace toadlet{
namespace peeper{

VertexBufferAccessor::VertexBufferAccessor():
	mVertexBuffer(NULL),
	mVertexSize32(0),
	mNativeFloat(false),
	mNativeFixed(false),
	mData(NULL),
	mFixedData(NULL),
	mFloatData(NULL),
	mColorData(NULL)
{
}

VertexBufferAccessor::VertexBufferAccessor(VertexBuffer *vertexBuffer,Buffer::LockType lockType):
	mVertexBuffer(NULL),
	mVertexSize32(0),
	mNativeFloat(false),
	mNativeFixed(false),
	mData(NULL),
	mFixedData(NULL),
	mFloatData(NULL),
	mColorData(NULL)
{
	lock(vertexBuffer,lockType);
}

VertexBufferAccessor::~VertexBufferAccessor(){
	unlock();
}

void VertexBufferAccessor::lock(VertexBuffer *vertexBuffer,Buffer::LockType lockType){
	unlock();

	mVertexBuffer=vertexBuffer;

	VertexFormat *vertexFormat=mVertexBuffer->getVertexFormat();
	mVertexSize32=vertexFormat->getVertexSize()/sizeof(int32);
	TOADLET_ASSERT(vertexFormat->getNumVertexElements()<16/*mElementOffsets*/);
	for(int i=0;i<vertexFormat->getNumVertexElements();++i){
		mElementOffsets32[i]=vertexFormat->getVertexElement(i).offset/sizeof(int32);
	}

	const VertexElement &position=vertexFormat->getVertexElementOfType(VertexElement::Type_POSITION);
	if((position.format&VertexElement::Format_BIT_FLOAT_32)>0){
		mNativeFloat=true;
	}
	else if((position.format&VertexElement::Format_BIT_FIXED_32)>0){
		mNativeFixed=true;
	}

	mData=mVertexBuffer->lock(lockType);
	if(mNativeFixed){
		mFixedData=(int32*)mData;
	}
	else{
		mFloatData=(float*)mData;
	}
	if(vertexFormat->hasVertexElementOfType(VertexElement::Type_COLOR)){
		mColorData=(uint32*)mData;
	}
}

void VertexBufferAccessor::unlock(){
	if(mVertexBuffer!=NULL){
		mVertexBuffer->unlock();
		mVertexBuffer=NULL;
		mData=NULL;
		mFixedData=NULL;
		mFloatData=NULL;
		mColorData=NULL;
	}
}

}
}
