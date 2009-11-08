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

#include "D3D9VertexBuffer.h"
#include "D3D9Renderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9VertexBuffer::D3D9VertexBuffer(D3D9Renderer *renderer):
	mRenderer(NULL),

	mUsageFlags(0),
	mAccessType(AccessType_NO_ACCESS),
	mSize(0),
	//mVertexFormat,
	mVertexSize(0),
	mDataSize(0),

	mFVF(0),
	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mVertexBuffer(NULL),
	//mColorElements,
	mLockType(AccessType_NO_ACCESS),
	mData(NULL),
	mBacking(false),
	mBackingData(NULL)
{
	mRenderer=renderer;
}

D3D9VertexBuffer::~D3D9VertexBuffer(){
	destroy();
}

bool D3D9VertexBuffer::create(int usageFlags,AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	destroy();

	mUsageFlags=usageFlags;
	mAccessType=accessType;
	mSize=size;
	mVertexFormat=vertexFormat;
	mVertexSize=mVertexFormat->vertexSize;
	mDataSize=mVertexSize*mSize;
	mFVF=getFVF(mVertexFormat,&mColorElements);

	return createContext();
}

void D3D9VertexBuffer::destroy(){
	destroyContext(false);

	if(mBackingData!=NULL){
		delete[] mBackingData;
		mBackingData=NULL;
	}
}

bool D3D9VertexBuffer::createContext(){
	mD3DUsage=0;
	mD3DPool=D3DPOOL_MANAGED;
	if((mUsageFlags&UsageFlags_DYNAMIC)>0){
		mD3DUsage|=D3DUSAGE_DYNAMIC;
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			mD3DPool=D3DPOOL_DEFAULT;
		#endif
	}

	if(mAccessType==AccessType_WRITE_ONLY){
		mD3DUsage|=D3DUSAGE_WRITEONLY;
	}

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_EXCESSIVE,
		String("Allocating D3D9VertexBuffer of size:")+mDataSize);

	HRESULT result=mRenderer->getDirect3DDevice9()->CreateVertexBuffer(mDataSize,mD3DUsage,mFVF,mD3DPool,&mVertexBuffer TOADLET_SHAREDHANDLE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: CreateVertexBuffer");

	return SUCCEEDED(result);
}

void D3D9VertexBuffer::destroyContext(bool backData){
	if(backData){
		mBackingData=new uint8[mDataSize];
		mBacking=true;

		uint8 *data=lock(AccessType_READ_ONLY);
		memcpy(mBackingData,data,mDataSize);
		unlock();
	}

	if(mVertexBuffer!=NULL){
		mVertexBuffer->Release();
		mVertexBuffer=NULL;
	}
}

bool D3D9VertexBuffer::contextNeedsReset(){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

uint8 *D3D9VertexBuffer::lock(AccessType lockType){
	mLockType=lockType;

	DWORD d3dflags=0;
	switch(mLockType){
		case AccessType_WRITE_ONLY:
			if((mUsageFlags&UsageFlags_DYNAMIC)>0){
				d3dflags|=D3DLOCK_DISCARD;
			}
		break;
		case AccessType_READ_WRITE:
			d3dflags|=D3DLOCK_READONLY;
		break;
	}

	HRESULT result=mVertexBuffer->Lock(0,0,(void**)&mData,d3dflags);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: Lock");

	// We do this even if its write only, since the unlocking will write it back, it would get messed up if we didn't swap in all situations
	if(mData!=NULL){
		int i,j;
		for(i=0;i<mColorElements.size();++i){
			const VertexElement &vertexElement=mColorElements[i];
			for(j=0;j<mSize;++j){
				uint32 &color=*(uint32*)(mData+mVertexSize*j+vertexElement.offset);
				color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
			}
		}
	}

	return mData;
}

bool D3D9VertexBuffer::unlock(){
	// We do this even if its read only, since we have to do it in all situations for locking
	if(mData!=NULL){
		int i,j;
		for(i=0;i<mColorElements.size();++i){
			const VertexElement &vertexElement=mColorElements[i];
			for(j=0;j<mSize;++j){
				uint32 &color=*(uint32*)(mData+mVertexSize*j+vertexElement.offset);
				color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
			}
		}
	}

	HRESULT result=mVertexBuffer->Unlock();
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: Unlock");

	mData=NULL;

	return SUCCEEDED(result);
}

DWORD D3D9VertexBuffer::getFVF(VertexFormat *vertexFormat,Collection<VertexElement> *colorElements){
	DWORD fvf=0;

	int i;
	int texCoordCount=0;
	for(i=0;i<vertexFormat->vertexElements.size();++i){
		const VertexElement &element=vertexFormat->vertexElements[i];
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
		else if(element.type==VertexElement::Type_COLOR_DIFFUSE && element.format==VertexElement::Format_COLOR_RGBA){
			fvf|=D3DFVF_DIFFUSE;
			colorElements->add(element);
		}
		else if(element.type==VertexElement::Type_COLOR_SPECULAR && element.format==VertexElement::Format_COLOR_RGBA){
			fvf|=D3DFVF_SPECULAR;
			colorElements->add(element);
		}
		else if(element.type>=VertexElement::Type_TEX_COORD){
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
						"D3D9VertexBuffer: Invalid tex coord count");
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
				String("D3DVertexBuffer: Invalid vertex element:")+element.type+","+element.format);
		}
	}

	switch(texCoordCount){
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
				String("D3DVertexBuffer: Invalid tex coord number")+texCoordCount);
	}

	return fvf;
}

}
}
