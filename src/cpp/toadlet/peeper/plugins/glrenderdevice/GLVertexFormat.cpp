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

namespace toadlet{
namespace peeper{

GLVertexFormat::GLVertexFormat(GLRenderDevice *renderDevice):
	mDevice(NULL),

	mListener(NULL),
	//mSemantics,
	//mIndexes,
	//mFormats,
	//mOffsets,
	mVertexSize(0)

	//mGLDataTypes,
	//mGLElementCount,
{
	mDevice=renderDevice;
}

GLVertexFormat::~GLVertexFormat(){
	destroy();
}

bool GLVertexFormat::create(){
	return true;
}

void GLVertexFormat::destroy(){
	mSemantics.clear();
	mIndexes.clear();
	mFormats.clear();
	mOffsets.clear();
	mGLDataTypes.clear();
	mGLElementCounts.clear();

	if(mListener!=NULL){
		mListener->vertexFormatDestroyed(this);
	}
}

void GLVertexFormat::addElement(int semantic,int index,int format){
	int offset=mVertexSize;
	mSemantics.add(semantic);
	mIndexes.add(index);
	mFormats.add(format);
	mOffsets.add(offset);

	mVertexSize+=getFormatSize(format);

	mGLDataTypes.add(GLRenderDevice::getGLDataType(format));
	mGLElementCounts.add(GLRenderDevice::getGLElementCount(format));
}

int GLVertexFormat::findSemantic(int semantic){
	TOADLET_ASSERT(mGLDataTypes.size()==mSemantics.size());

	int i;
	for(i=0;i<mSemantics.size();++i){
		if(mSemantics[i]==semantic){
			return i;
		}
	}
	return -1;
}

}
}
