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

#include "D3D9VertexBufferPeer.h"
#include "D3D9Renderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9VertexBufferPeer::D3D9VertexBufferPeer(D3D9Renderer *renderer,VertexBuffer *buffer):
	usageType(Buffer::UsageType_STATIC)
	,vertexBuffer(NULL)
	,fvf(0)
	,data(NULL)

	,lockType(Buffer::LockType_READ_ONLY)
	,size(0)
	,vertexSize(0)
	//,colorElements
{
	usageType=buffer->getUsageType();
	size=buffer->getSize();
	vertexSize=buffer->getVertexFormat()->getVertexSize();

	fvf=getFVF(buffer,&colorElements);

	// TODO: Try to unify this
	DWORD d3dUsage=0;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		D3DMPOOL d3dPool=D3DMPOOL_SYSTEMMEM;
	#else
		D3DPOOL d3dPool=D3DPOOL_MANAGED;
	#endif
	if(buffer->getUsageType()==Buffer::UsageType_DYNAMIC){
		d3dUsage|=D3DUSAGE_DYNAMIC;
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			d3dPool=D3DPOOL_DEFAULT;
		#endif
	}
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		else if(renderer->getD3DCAPS9().SurfaceCaps & D3DMSURFCAPS_VIDVERTEXBUFFER){
			d3dPool=D3DMPOOL_VIDEOMEM;
		}
	#endif

	if(buffer->getAccessType()==Buffer::AccessType_WRITE_ONLY){
		d3dUsage|=D3DUSAGE_WRITEONLY;
	}

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_EXCESSIVE,
		String("Allocating D3D9VertexBufferPeer of size:")+buffer->getBufferSize());

	HRESULT result=renderer->getDirect3DDevice9()->CreateVertexBuffer(buffer->getBufferSize(),d3dUsage,fvf,d3dPool,&vertexBuffer TOADLET_SHAREDHANDLE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBufferPeer: CreateVertexBuffer");

	uint8 *bufferData=buffer->internal_getData();
	if(bufferData!=NULL){
		uint8 *data=lock(Buffer::LockType_WRITE_ONLY);
		if(data!=NULL){
			memcpy(data,bufferData,buffer->getBufferSize());
		}
		unlock();
	}
}

D3D9VertexBufferPeer::~D3D9VertexBufferPeer(){
	if(vertexBuffer!=NULL){
		vertexBuffer->Release();
		vertexBuffer=NULL;
	}
}

uint8 *D3D9VertexBufferPeer::lock(Buffer::LockType lockType){
	DWORD flags=0;
	switch(lockType){
		case Buffer::LockType_WRITE_ONLY:
			if(usageType==Buffer::UsageType_DYNAMIC){
				flags|=D3DLOCK_DISCARD;
			}
		break;
		case Buffer::LockType_READ_WRITE:
			flags|=D3DLOCK_READONLY;
		break;
	}

	this->lockType=lockType;
	HRESULT result=vertexBuffer->Lock(0,0,(void**)&data,flags);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBufferPeer: Lock");

	if(data!=NULL){
		if(lockType!=Buffer::LockType_WRITE_ONLY){
			int i,j;
			for(i=0;i<colorElements.size();++i){
				const VertexElement &vertexElement=colorElements[i];
				for(j=0;j<size;++j){
					uint32 &color=*(uint32*)(data+vertexSize*j+vertexElement.offset);
					color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
				}
			}
		}
	}

	return data;
}

bool D3D9VertexBufferPeer::unlock(){
	if(lockType!=Buffer::LockType_READ_ONLY){
		int i,j;
		for(i=0;i<colorElements.size();++i){
			const VertexElement &vertexElement=colorElements[i];
			for(j=0;j<size;++j){
				uint32 &color=*(uint32*)(data+vertexSize*j+vertexElement.offset);
				color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
			}
		}
	}

	HRESULT result=vertexBuffer->Unlock();
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBufferPeer: Unlock");

	data=NULL;

	return true;
}

DWORD D3D9VertexBufferPeer::getFVF(VertexBuffer *buffer,Collection<VertexElement> *colorElements){
	VertexFormat *vertexFormat=buffer->getVertexFormat();

	DWORD fvf=0;

	switch(vertexFormat->getMaxTexCoordIndex()+1){
		case 0:
			fvf|=D3DFVF_TEX0;
		break;
		case 1:
			fvf|=D3DFVF_TEX1;
		break;
		case 2:
			fvf|=D3DFVF_TEX2;
		break;
		case 3:
			fvf|=D3DFVF_TEX3;
		break;
		case 4:
			fvf|=D3DFVF_TEX4;
		break;
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			case 5:
				fvf|=D3DFVF_TEX5;
			break;
			case 6:
				fvf|=D3DFVF_TEX6;
			break;
			case 7:
				fvf|=D3DFVF_TEX7;
			break;
			case 8:
				fvf|=D3DFVF_TEX8;
			break;
		#endif
		default:
			Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
				String("D3DVertexBufferPeer: Invalid tex coord number")+(vertexFormat->getMaxTexCoordIndex()+1));
	}

	int i;
	int texCoordCount=0;
	for(i=0;i<vertexFormat->getNumVertexElements();++i){
		const VertexElement &element=vertexFormat->getVertexElement(i);
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
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
		#else
			if(element.type==VertexElement::Type_POSITION && element.format==(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3)){
				fvf|=D3DFVF_XYZ;
			}
			else if(element.type==VertexElement::Type_NORMAL && element.format==(VertexElement::Format_BIT_FLOAT_32|VertexElement::Format_BIT_COUNT_3)){
				fvf|=D3DFVF_NORMAL;
			}
		#endif
		else if(element.type==VertexElement::Type_COLOR && element.format==VertexElement::Format_COLOR_RGBA){
			fvf|=D3DFVF_DIFFUSE;
			if(colorElements!=NULL){
				colorElements->add(element);
			}
		}
		else if(element.type==VertexElement::Type_TEX_COORD){
			if((element.format&VertexElement::Format_BIT_COUNT_1)>0){
				fvf|=D3DFVF_TEXCOORDSIZE1(texCoordCount);
			}
			else if((element.format&VertexElement::Format_BIT_COUNT_2)>0){
				fvf|=D3DFVF_TEXCOORDSIZE2(texCoordCount);
			}
			else if((element.format&VertexElement::Format_BIT_COUNT_3)>0){
				fvf|=D3DFVF_TEXCOORDSIZE3(texCoordCount);
			}
			else if((element.format&VertexElement::Format_BIT_COUNT_4)>0){
				#if defined(TOADLET_HAS_DIRECT3DMOBILE)
					Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
						"D3D9VertexBufferPeer: Invalid tex coord count");
				#else
					fvf|=D3DFVF_TEXCOORDSIZE4(texCoordCount);
				#endif
			}

			#if defined(TOADLET_HAS_DIRECT3DMOBILE)
				if((element.format&VertexElement::Format_BIT_FIXED_32)>0){
					fvf|=D3DMFVF_TEXCOORDFIXED(texCoordCount);
				}
				else if((element.format&VertexElement::Format_BIT_FLOAT_32)>0){
					fvf|=D3DMFVF_TEXCOORDFLOAT(texCoordCount);
				}
			#endif

			texCoordCount++;
		}
		else{
			Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
				String("D3DVertexBufferPeer: Invalid vertex element:")+element.type+","+element.format);
		}
	}

	return fvf;
}

}
}
