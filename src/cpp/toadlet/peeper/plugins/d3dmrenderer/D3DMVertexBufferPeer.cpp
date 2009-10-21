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

#include "D3DMVertexBufferPeer.h"
#include "D3DMRenderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3DMVertexBufferPeer::D3DMVertexBufferPeer(D3DMRenderer *renderer,VertexBuffer *buffer):
	usageType(Buffer::UsageType_STATIC)
	,vertexBuffer(NULL)
	,fvf(0)
	,data(NULL)

	,lockType(Buffer::LockType_READ_ONLY)
	,numVertexes(0)
	,vertexSize(0)
	//,colorElements
{
	usageType=buffer->getUsageType();
	numVertexes=buffer->getSize();
	vertexSize=buffer->getVertexFormat()->getVertexSize();

	fvf=getFVF(buffer,&colorElements);

	DWORD d3dUsage=0;
	D3DMPOOL d3dPool=D3DMPOOL_SYSTEMMEM;
	if(buffer->getUsageType()==Buffer::UsageType_DYNAMIC){
		d3dUsage|=D3DMUSAGE_DYNAMIC;
	}
	else if(renderer->getD3DMCAPS().SurfaceCaps & D3DMSURFCAPS_VIDVERTEXBUFFER){
		d3dPool=D3DMPOOL_VIDEOMEM;
	}

	if(buffer->getAccessType()==Buffer::AccessType_WRITE_ONLY){
		d3dUsage|=D3DMUSAGE_WRITEONLY;
	}

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_EXCESSIVE,
		String("Allocating D3DMVertexBufferPeer of size:")+buffer->getBufferSize());

	HRESULT result=renderer->getDirect3DMobileDevice()->CreateVertexBuffer(buffer->getBufferSize(),d3dUsage,fvf,d3dPool,&vertexBuffer);
	TOADLET_CHECK_D3DMERROR(result,"D3DMVertexBufferPeer: CreateVertexBuffer");

	uint8 *bufferData=buffer->internal_getData();
	if(bufferData!=NULL){
		uint8 *data=lock(Buffer::LockType_WRITE_ONLY);
		if(data!=NULL){
			memcpy(data,bufferData,buffer->getBufferSize());
		}
		unlock();
	}
}

D3DMVertexBufferPeer::~D3DMVertexBufferPeer(){
	if(vertexBuffer!=NULL){
		vertexBuffer->Release();
		vertexBuffer=NULL;
	}
}

uint8 *D3DMVertexBufferPeer::lock(Buffer::LockType lockType){
	DWORD flags=0;
	switch(lockType){
		case Buffer::LockType_WRITE_ONLY:
			if(usageType==Buffer::UsageType_DYNAMIC){
				flags|=D3DMLOCK_DISCARD;
			}
		break;
		case Buffer::LockType_READ_WRITE:
			flags|=D3DMLOCK_READONLY;
		break;
	}

	HRESULT result=vertexBuffer->Lock(0,0,(void**)&data,flags);
	TOADLET_CHECK_D3DMERROR(result,"D3DMVertexBufferPeer: Lock");

	return data;
}

bool D3DMVertexBufferPeer::unlock(){
	if(lockType!=Buffer::LockType_READ_ONLY){
		int i,j;
		for(i=0;i<colorElements.size();++i){
			const VertexElement &vertexElement=colorElements[i];
			for(j=0;j<numVertexes;++j){
				uint32 &color=*(uint32*)(data+vertexSize*j+vertexElement.offset);
				color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
			}
		}
	}

	HRESULT result=vertexBuffer->Unlock();
	TOADLET_CHECK_D3DMERROR(result,"D3DMVertexBufferPeer: Unlock");

	data=NULL;

	return true;
}

DWORD D3DMVertexBufferPeer::getFVF(VertexBuffer *buffer,Collection<VertexElement> *colorElements){
	VertexFormat *vertexFormat=buffer->getVertexFormat();

	DWORD fvf=0;

	switch(vertexFormat->getMaxTexCoordIndex()+1){
		case 0:
			fvf|=D3DMFVF_TEX0;
		break;
		case 1:
			fvf|=D3DMFVF_TEX1;
		break;
		case 2:
			fvf|=D3DMFVF_TEX2;
		break;
		case 3:
			fvf|=D3DMFVF_TEX3;
		break;
		case 4:
			fvf|=D3DMFVF_TEX4;
		break;
		default:
			Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
				"D3DMVertexBufferPeer: Invalid tex coord number");
	}

	int i;
	int texCoordCount=0;
	for(i=0;i<vertexFormat->getNumVertexElements();++i){
		const VertexElement &element=vertexFormat->getVertexElement(i);
		if(element.type==VertexElement::Type_POSITION && element.format==(VertexElement::Format_BIT_FIXED_32|VertexElement::Format_BIT_COUNT_3)){
			fvf|=D3DMFVF_XYZ_FIXED;
		}
		else if(element.type==VertexElement::Type_POSITION && element.format==(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3)){
			fvf|=D3DMFVF_XYZ_FLOAT;
		}
		else if(element.type==VertexElement::Type_NORMAL && element.format==(VertexElement::Format_BIT_FIXED_32|VertexElement::Format_BIT_COUNT_3)){
			fvf|=D3DMFVF_NORMAL_FIXED;
		}
		else if(element.type==VertexElement::Type_NORMAL && element.format==(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3)){
			fvf|=D3DMFVF_NORMAL_FLOAT;
		}
		else if(element.type==VertexElement::Type_COLOR && element.format==VertexElement::Format_COLOR_RGBA){
			fvf|=D3DMFVF_DIFFUSE;
			colorElements->add(element);
		}
		else if(element.type==VertexElement::Type_TEX_COORD){
			if((element.format&VertexElement::Format_BIT_COUNT_1)>0){
				fvf|=D3DMFVF_TEXCOORDSIZE1(texCoordCount);
			}
			else if((element.format&VertexElement::Format_BIT_COUNT_2)>0){
				fvf|=D3DMFVF_TEXCOORDSIZE2(texCoordCount);
			}
			else if((element.format&VertexElement::Format_BIT_COUNT_3)>0){
				fvf|=D3DMFVF_TEXCOORDSIZE3(texCoordCount);
			}
			else if((element.format&VertexElement::Format_BIT_COUNT_4)>0){
				Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
					"D3DMVertexBufferPeer: Invalid tex coord count");
			}

			if((element.format&VertexElement::Format_BIT_FIXED_32)>0){
				fvf|=D3DMFVF_TEXCOORDFIXED(texCoordCount);
			}
			else if((element.format&VertexElement::Format_BIT_FLOAT_32)>0){
				fvf|=D3DMFVF_TEXCOORDFLOAT(texCoordCount);
			}

			texCoordCount++;
		}
		else{
			Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
				"D3DMVertexBufferPeer: Invalid vertex element");
		}
	}

	return fvf;
}

}
}
