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

#include "D3D9IndexBufferPeer.h"
#include "D3D9Renderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9IndexBufferPeer::D3D9IndexBufferPeer(D3D9Renderer *renderer,IndexBuffer *buffer):
	usageType(Buffer::UsageType_STATIC),
	indexBuffer(NULL),
	data(NULL),

	lockType(Buffer::LockType_READ_ONLY),
	size(0),
	format(IndexBuffer::IndexFormat_UINT_8)
{
	usageType=buffer->getUsageType();

	size=buffer->getSize();
	format=buffer->getIndexFormat();
	D3DFORMAT d3dFormat=getD3DFORMAT(format);

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
		else if(renderer->getD3DCAPS9().SurfaceCaps & D3DMSURFCAPS_VIDINDEXBUFFER){
			d3dPool=D3DMPOOL_VIDEOMEM;
		}
	#endif

	if(buffer->getAccessType()==Buffer::AccessType_WRITE_ONLY){
		d3dUsage|=D3DUSAGE_WRITEONLY;
	}

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_EXCESSIVE,
		String("Allocating D3D9IndexBufferPeer of size:")+buffer->getBufferSize());

	int bufferSize=buffer->getBufferSize();
	if(format==IndexBuffer::IndexFormat_UINT_8) bufferSize*=2;
	HRESULT result=renderer->getDirect3DDevice9()->CreateIndexBuffer(bufferSize,d3dUsage,d3dFormat,d3dPool,&indexBuffer TOADLET_SHAREDHANDLE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9IndexBufferPeer: CreateIndexBuffer");

	uint8 *bufferData=buffer->internal_getData();
	if(bufferData!=NULL){
		uint8 *data=lock(Buffer::LockType_WRITE_ONLY);
		if(data!=NULL){
			memcpy(data,bufferData,buffer->getBufferSize());
		}
		unlock();
	}
}

D3D9IndexBufferPeer::~D3D9IndexBufferPeer(){
	if(indexBuffer!=NULL){
		indexBuffer->Release();
		indexBuffer=NULL;
	}
}

uint8 *D3D9IndexBufferPeer::lock(Buffer::LockType lockType){
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
	HRESULT result=indexBuffer->Lock(0,0,(void**)&data,flags);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9IndexBufferPeer: Lock");

	// Pack the indexes
	if(lockType!=Buffer::LockType_WRITE_ONLY && format==IndexBuffer::IndexFormat_UINT_8){
		uint16 *data16=(uint16*)data;
		int i;
		for(i=0;i<size;++i){
			data[i]=data16[i];
		}
	}

	return data;
}

bool D3D9IndexBufferPeer::unlock(){
	// Unpack the indexes
	if(lockType!=Buffer::LockType_READ_ONLY && format==IndexBuffer::IndexFormat_UINT_8){
		uint16 *data16=(uint16*)data;
		int i;
		for(i=size-1;i>=0;--i){
			data16[i]=data[i];
		}
	}

	HRESULT result=indexBuffer->Unlock();
	TOADLET_CHECK_D3D9ERROR(result,"D3D9IndexBufferPeer: Unlock");

	data=NULL;

	return true;
}

D3DFORMAT D3D9IndexBufferPeer::getD3DFORMAT(IndexBuffer::IndexFormat format){
	D3DFORMAT d3dFormat=(D3DFORMAT)0;
	switch(format){
		case IndexBuffer::IndexFormat_UINT_8:
		case IndexBuffer::IndexFormat_UINT_16:
			d3dFormat=D3DFMT_INDEX16;
		break;
		case IndexBuffer::IndexFormat_UINT_32:
			d3dFormat=D3DFMT_INDEX32;
		break;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9IndexBufferPeer::getD3DFORMAT: Invalid format");
			return (D3DFORMAT)0;
		break;
	}

	return d3dFormat;
}

}
}
