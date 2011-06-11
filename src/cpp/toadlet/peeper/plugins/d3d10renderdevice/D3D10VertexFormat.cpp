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

#include "D3D10VertexFormat.h"
#include "D3D10RenderDevice.h"
#include <toadlet/peeper/BackableVertexFormat.h>

#include "D3D10Shader.h"

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D10VertexFormat::D3D10VertexFormat(D3D10RenderDevice *renderDevice):
	mDevice(NULL),

	mListener(NULL),
	//mSemantics,
	//mFormats,
	//mOffsets,
	mVertexSize(0),

	mUniqueHandle(0)
	//mElements
{
	mDevice=renderDevice;
}

D3D10VertexFormat::~D3D10VertexFormat(){
	destroy();
}

bool D3D10VertexFormat::create(){
	if(mDevice!=NULL){
		mDevice->vertexFormatCreated(this);
	}

	return true;
}

void D3D10VertexFormat::destroy(){
	mSemantics.clear();
	mNames.clear();
	mIndexes.clear();
	mFormats.clear();
	mOffsets.clear();
	mElements.clear();

	if(mListener!=NULL){
		mListener->vertexFormatDestroyed(this);
	}

	if(mDevice!=NULL){
		mDevice->vertexFormatDestroyed(this);
	}
}

bool D3D10VertexFormat::addElement(int semantic,const String &name,int index,int format){
	int offset=mVertexSize;
	if(semantic==Semantic_UNKNOWN){
		mSemantics.add(BackableVertexFormat::getSemanticFromName(name));
	}
	else{
		mSemantics.add(semantic);
	}
	if(name==(char*)NULL){
		mNames.add(BackableVertexFormat::getNameFromSemantic(semantic));
	}
	else{
		mNames.add(name);
	}
	mIndexes.add(index);
	mFormats.add(format);
	mOffsets.add(offset);

	mVertexSize+=getFormatSize(format);

	D3D10_INPUT_ELEMENT_DESC element={
		NULL,index,D3D10RenderDevice::getVertexDXGI_FORMAT(format),0,offset,D3D10_INPUT_PER_VERTEX_DATA,0
	};
	mElements.add(element);

	// Reassign char pointers because the vector realllocates memory
	int i;
	for(i=0;i<mElements.size();++i){
		mElements[i].SemanticName=mNames[i];
	}

	return true;
}

int D3D10VertexFormat::findElement(int semantic){
	int i;
	for(i=0;i<mSemantics.size();++i){
		if(mSemantics[i]==semantic){
			return i;
		}
	}
	return -1;
}

int D3D10VertexFormat::findElement(const String &name){
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
