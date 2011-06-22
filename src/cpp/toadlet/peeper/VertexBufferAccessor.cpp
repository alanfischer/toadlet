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

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

VertexBufferAccessor::VertexBufferAccessor():
	mVertexBuffer(NULL),
	mVertexSize32(0),
	mNativeFixed(false),
	mData(NULL),
	mFixedData(NULL),
	mFloatData(NULL),
	mColorData(NULL)
{}

VertexBufferAccessor::VertexBufferAccessor(VertexBuffer *vertexBuffer,int access):
	mVertexBuffer(NULL),
	mVertexSize32(0),
	mNativeFixed(false),
	mData(NULL),
	mFixedData(NULL),
	mFloatData(NULL),
	mColorData(NULL)
{
	lock(vertexBuffer,access);
}

VertexBufferAccessor::~VertexBufferAccessor(){
	unlock();
}

void VertexBufferAccessor::lock(VertexBuffer *vertexBuffer,int access){
	unlock();

	mVertexBuffer=vertexBuffer;
	VertexFormat *vertexFormat=mVertexBuffer->getVertexFormat();
	mVertexSize32=vertexFormat->getVertexSize()/sizeof(int32);
	int numElements=vertexFormat->getNumElements();
	TOADLET_ASSERT(numElements>0 && numElements<16);
	for(int i=0;i<numElements;++i){
		mElementOffsets32[i]=vertexFormat->getOffset(i)/sizeof(int32);
	}

	if((vertexFormat->getFormat(0)&VertexFormat::Format_MASK_TYPES)==VertexFormat::Format_TYPE_FIXED_32){
		mNativeFixed=true;
	}
	else{
		mNativeFixed=false;
	}

	mData=mVertexBuffer->lock(access);
	if(mNativeFixed){
		mFixedData=(int32*)mData;
	}
	else{
		mFloatData=(float*)mData;
	}
	if(vertexFormat->findElement(VertexFormat::Semantic_COLOR)>0){
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
