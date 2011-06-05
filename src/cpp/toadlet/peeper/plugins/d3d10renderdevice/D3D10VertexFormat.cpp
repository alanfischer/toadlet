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
#include <toadlet/egg/Error.h>

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

	//mElements,
	mLayout(NULL)
{
	mDevice=renderDevice;
}

D3D10VertexFormat::~D3D10VertexFormat(){
	destroy();
}

bool D3D10VertexFormat::create(){
	return true;
}

void D3D10VertexFormat::destroy(){
	destroyContext();

	mSemantics.clear();
	mIndexes.clear();
	mFormats.clear();
	mOffsets.clear();
	mElements.clear();

	if(mListener!=NULL){
		mListener->vertexFormatDestroyed(this);
	}
}

bool D3D10VertexFormat::createContext(){
	HRESULT result=mDevice->getD3D10Device()->CreateInputLayout(
		mElements,mElements.size(),mDevice->passDesc.pIAInputSignature,mDevice->passDesc.IAInputSignatureSize,&mLayout
	);

	return SUCCEEDED(result);
}

bool D3D10VertexFormat::destroyContext(){
	if(mLayout!=NULL){
		mLayout->Release();
		mLayout=NULL;
	}
	
	return true;
}

void D3D10VertexFormat::addElement(int semantic,int index,int format){
	int offset=mVertexSize;
	mSemantics.add(semantic);
	mIndexes.add(index);
	mFormats.add(format);
	mOffsets.add(offset);

	mVertexSize+=getFormatSize(format);

	D3D10_INPUT_ELEMENT_DESC element={
		D3D10RenderDevice::getSemanticName(semantic),index,D3D10RenderDevice::getVertexDXGI_FORMAT(format),0,offset,D3D10_INPUT_PER_VERTEX_DATA,0
	};
	mElements.add(element);

	destroyContext();
}

int D3D10VertexFormat::findSemantic(int semantic){
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
