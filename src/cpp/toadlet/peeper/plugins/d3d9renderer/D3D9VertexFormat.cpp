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

#include "D3D9VertexFormat.h"
#include "D3D9Renderer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9VertexFormat::D3D9VertexFormat(D3D9Renderer *renderer):
	mRenderer(NULL),

	//mSemantics,
	//mIndexes,
	//mFormats,
	//mOffsets,
	mVertexSize(0),

	mFVF(0)
	#if !defined(TOADLET_SET_D3DM)
		,mElements(NULL),
		mDeclaration(NULL)
	#endif
{
	mRenderer=renderer;
}

D3D9VertexFormat::~D3D9VertexFormat(){
	destroy();
}

void D3D9VertexFormat::addElement(int semantic,int index,int format){
	mSemantics.add(semantic);
	mIndexes.add(index);
	mFormats.add(format);
	mOffsets.add(mVertexSize);

	mVertexSize+=getFormatSize(format);
}

bool D3D9VertexFormat::create(){
	destroy();

	mFVF=D3D9Renderer::getFVF(this);

	HRESULT result=S_OK;
	#if !defined(TOADLET_SET_D3DM)
		D3DVERTEXELEMENT9 *elements=new D3DVERTEXELEMENT9[mSemantics.size()+1];
		int i;
		for(i=0;i<mSemantics.size();++i){
			D3DVERTEXELEMENT9 element={
				0,mOffsets[i],getD3DDECLTYPE(mFormats[i]),D3DDECLMETHOD_DEFAULT,getD3DDECLUSAGE(mSemantics[i]),mIndexes[i]
			};
			elements[i]=element;
		}
		D3DVERTEXELEMENT9 element=D3DDECL_END();
		elements[mSemantics.size()]=element;
		mElements=elements;

		result=mRenderer->getDirect3DDevice9()->CreateVertexDeclaration(mElements,&mDeclaration);
		TOADLET_CHECK_D3D9ERROR(result,"CreateVertexDeclaration");
	#endif

	return SUCCEEDED(result);
}

void D3D9VertexFormat::destroy(){
	#if !defined(TOADLET_SET_D3DM)
		if(mElements!=NULL){
			delete[] mElements;
			mElements=NULL;
		}

		if(mDeclaration!=NULL){
			mDeclaration->Release();
			mDeclaration=NULL;
		}
	#endif
}

int D3D9VertexFormat::findSemantic(int semantic){
	TOADLET_ASSERT(mFVF!=0);

	int i;
	for(i=0;i<mSemantics.size();++i){
		if(mSemantics[i]==semantic){
			return i;
		}
	}
	return -1;
}

#if !defined(TOADLET_SET_D3DM)
	BYTE D3D9VertexFormat::getD3DDECLTYPE(int format){
		if(format==Format_COLOR_RGBA){
			return D3DDECLTYPE_D3DCOLOR;
		}
		else{
			switch(format){
				case Format_BIT_FLOAT_32|Format_BIT_COUNT_1:
					return D3DDECLTYPE_FLOAT1;
				case Format_BIT_FLOAT_32|Format_BIT_COUNT_2:
					return D3DDECLTYPE_FLOAT2;
				case Format_BIT_FLOAT_32|Format_BIT_COUNT_3:
					return D3DDECLTYPE_FLOAT3;
				case Format_BIT_FLOAT_32|Format_BIT_COUNT_4:
					return D3DDECLTYPE_FLOAT4;
				case Format_BIT_INT_16|Format_BIT_COUNT_2:
					return D3DDECLTYPE_SHORT2;
				case Format_BIT_INT_16|Format_BIT_COUNT_4:
					return D3DDECLTYPE_SHORT4;
				case Format_BIT_UINT_8|Format_BIT_COUNT_4:
					return D3DDECLTYPE_UBYTE4;
				default:
					Error::unknown("unknown D3DDECLTYPE");
					return -1;
			}
		}
	}

	BYTE D3D9VertexFormat::getD3DDECLUSAGE(int semantic){
		switch(semantic){
			case Semantic_POSITION:
				return D3DDECLUSAGE_POSITION;
			case Semantic_BLEND_WEIGHTS:
				return D3DDECLUSAGE_BLENDWEIGHT;
			case Semantic_BLEND_INDICES:
				return D3DDECLUSAGE_BLENDINDICES;
			case Semantic_NORMAL:
				return D3DDECLUSAGE_NORMAL;
			case Semantic_COLOR:
				return D3DDECLUSAGE_COLOR;
			case Semantic_TEX_COORD:
				return D3DDECLUSAGE_TEXCOORD;
			default:
				Error::unknown("unknown D3DDECLUSAGE");
				return -1;
		}
	}
#endif

}
}
