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
#include "D3D10Renderer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D10VertexFormat::D3D10VertexFormat(D3D10Renderer *renderer):
	mRenderer(NULL),

	//mSemantics,
	//mFormats,
	//mOffsets,
	mVertexSize(0),

	mElements(NULL),
	mLayout(NULL)
{
	mRenderer=renderer;
}

D3D10VertexFormat::~D3D10VertexFormat(){
	destroy();
}

void D3D10VertexFormat::addElement(int semantic,int index,int format){
	mSemantics.add(semantic);
	mIndexes.add(index);
	mFormats.add(format);
	mOffsets.add(mVertexSize);

	mVertexSize+=getFormatSize(format);
}

bool D3D10VertexFormat::create(){
	destroy();

	D3D10_INPUT_ELEMENT_DESC *elements=new D3D10_INPUT_ELEMENT_DESC[mSemantics.size()];
	int i;
	for(i=0;i<mSemantics.size();++i){
		D3D10_INPUT_ELEMENT_DESC element={
			D3D10Renderer::getSemanticName(mSemantics[i]),mIndexes[i],D3D10Renderer::getVertexDXGI_FORMAT(mFormats[i]),0,mOffsets[i],D3D10_INPUT_PER_VERTEX_DATA,0
		};
		elements[i]=element;
	}
	mElements=elements;

	HRESULT result=mRenderer->getD3D10Device()->CreateInputLayout(
		mElements,mSemantics.size(),mRenderer->passDesc.pIAInputSignature,mRenderer->passDesc.IAInputSignatureSize,&mLayout
	);

	return SUCCEEDED(result);
}

void D3D10VertexFormat::destroy(){
	if(mElements!=NULL){
		delete[] mElements;
		mElements=NULL;
	}

	if(mLayout!=NULL){
		mLayout->Release();
		mLayout=NULL;
	}
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
