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

#include "GLVertexFormat.h"
#include "GLRenderDevice.h"
#include <toadlet/peeper/BackableVertexFormat.h>

namespace toadlet{
namespace peeper{

GLVertexFormat::GLVertexFormat(GLRenderDevice *renderDevice):
	mDevice(NULL),

	//mSemantics,
	//mNames,
	//mIndexes,
	//mFormats,
	//mOffsets,
	mVertexSize(0),

	mRenderHandle(0),
	//mGLDataTypes,
	//mGLElementCount,
	mSemanticBits(0)
{
	mDevice=renderDevice;
}

bool GLVertexFormat::create(){
	if(mDevice!=NULL){
		mDevice->vertexFormatCreated(this);
	}

	return true;
}

void GLVertexFormat::destroy(){
	mSemantics.clear();
	mNames.clear();
	mIndexes.clear();
	mFormats.clear();
	mOffsets.clear();
	mGLDataTypes.clear();
	mGLElementCounts.clear();

	if(mDevice!=NULL){
		mDevice->vertexFormatDestroyed(this);
		mDevice=NULL;
	}

	BaseResource::destroy();
}

bool GLVertexFormat::addElement(int semantic,const String &name,int index,int format){
	int offset=mVertexSize;
	if(semantic==Semantic_UNKNOWN){
		mSemantics.push_back(BackableVertexFormat::getSemanticFromName(name));
	}
	else{
		mSemantics.push_back(semantic);
	}
	if(name==(char*)NULL){
		mNames.push_back(BackableVertexFormat::getNameFromSemantic(semantic));
	}
	else{
		mNames.push_back(name);
	}
	mIndexes.push_back(index);
	mFormats.push_back(format);
	mOffsets.push_back(offset);

	mVertexSize+=getFormatSize(format);

	mGLDataTypes.push_back(GLRenderDevice::getGLDataType(format));
	mGLElementCounts.push_back(GLRenderDevice::getGLElementCount(format));
	mSemanticBits|=(1<<semantic);

	return true;
}

int GLVertexFormat::findElement(int semantic){
	TOADLET_ASSERT(mGLDataTypes.size()==mSemantics.size());

	int i;
	for(i=0;i<mSemantics.size();++i){
		if(mSemantics[i]==semantic){
			return i;
		}
	}
	return -1;
}

int GLVertexFormat::findElement(const String &name){
	TOADLET_ASSERT(mGLDataTypes.size()==mNames.size());

	int i;
	for(i=0;i<mNames.size();++i){
		if(mNames[i].equals(name)){
			return i;
		}
	}
	return -1;
}

}
}
