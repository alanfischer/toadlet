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
{
}

VertexBufferAccessor::VertexBufferAccessor(VertexBuffer *vertexBuffer,Buffer::AccessType accessType):
	mVertexBuffer(NULL),
	mVertexSize32(0),
	mNativeFixed(false),
	mData(NULL),
	mFixedData(NULL),
	mFloatData(NULL),
	mColorData(NULL)
{
	lock(vertexBuffer,accessType);
}

VertexBufferAccessor::~VertexBufferAccessor(){
	unlock();
}

void VertexBufferAccessor::lock(VertexBuffer *vertexBuffer,Buffer::AccessType accessType){
	unlock();

	mVertexBuffer=vertexBuffer;
	VertexFormat *vertexFormat=mVertexBuffer->getVertexFormat();
	mVertexSize32=vertexFormat->vertexSize/sizeof(int32);
	TOADLET_ASSERT(vertexFormat->vertexElements.size()>0 && vertexFormat->vertexElements.size()<16);
	for(int i=0;i<vertexFormat->vertexElements.size();++i){
		mElementOffsets32[i]=vertexFormat->vertexElements[i].offset/sizeof(int32);
	}

	if((vertexFormat->vertexElements[0].format&VertexElement::Format_BIT_FIXED_32)>0){
		mNativeFixed=true;
	}
	else{
		mNativeFixed=false;
	}

	mData=mVertexBuffer->lock(accessType);
	if(mNativeFixed){
		mFixedData=(int32*)mData;
	}
	else{
		mFloatData=(float*)mData;
	}
	if(vertexFormat->hasVertexElementOfType(VertexElement::Type_COLOR_DIFFUSE)){
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
