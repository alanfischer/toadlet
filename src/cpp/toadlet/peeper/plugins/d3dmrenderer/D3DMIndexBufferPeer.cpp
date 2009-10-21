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

#include "D3DMIndexBufferPeer.h"
#include "D3DMRenderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3DMIndexBufferPeer::D3DMIndexBufferPeer(D3DMRenderer *renderer,IndexBuffer *buffer):
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
	D3DMFORMAT d3dFormat=getD3DMFORMAT(format);

	DWORD d3dUsage=0;
	D3DMPOOL d3dPool=D3DMPOOL_SYSTEMMEM;
	if(buffer->getUsageType()==Buffer::UsageType_DYNAMIC){
		d3dUsage|=D3DMUSAGE_DYNAMIC;
	}
	else if(renderer->getD3DMCAPS().SurfaceCaps & D3DMSURFCAPS_VIDINDEXBUFFER){
		d3dPool=D3DMPOOL_VIDEOMEM;
	}

	if(buffer->getAccessType()==Buffer::AccessType_WRITE_ONLY){
		d3dUsage|=D3DMUSAGE_WRITEONLY;
	}

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_EXCESSIVE,
		String("Allocating D3DMIndexBufferPeer of size:")+buffer->getBufferSize());

	int bufferSize=buffer->getBufferSize();
	if(format==IndexBuffer::IndexFormat_UINT_8) bufferSize*=2;
	HRESULT result=renderer->getDirect3DMobileDevice()->CreateIndexBuffer(bufferSize,d3dUsage,d3dFormat,d3dPool,&indexBuffer);
	TOADLET_CHECK_D3DMERROR(result,"D3DMIndexBufferPeer: CreateIndexBuffer");

	uint8 *bufferData=buffer->internal_getData();
	if(bufferData!=NULL){
		uint8 *data=lock(Buffer::LockType_WRITE_ONLY);
		if(data!=NULL){
			memcpy(data,bufferData,buffer->getBufferSize());
		}
		unlock();
	}
}

D3DMIndexBufferPeer::~D3DMIndexBufferPeer(){
	if(indexBuffer!=NULL){
		indexBuffer->Release();
		indexBuffer=NULL;
	}
}

uint8 *D3DMIndexBufferPeer::lock(Buffer::LockType lockType){
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

	this->lockType=lockType;
	HRESULT result=indexBuffer->Lock(0,0,(void**)&data,flags);
	TOADLET_CHECK_D3DMERROR(result,"D3DMIndexBufferPeer: Lock");

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

bool D3DMIndexBufferPeer::unlock(){
	// Unpack the indexes
	if(lockType!=Buffer::LockType_READ_ONLY && format==IndexBuffer::IndexFormat_UINT_8){
		uint16 *data16=(uint16*)data;
		int i;
		for(i=size-1;i>=0;--i){
			data16[i]=data[i];
		}
	}

	HRESULT result=indexBuffer->Unlock();
	TOADLET_CHECK_D3DMERROR(result,"D3DMIndexBufferPeer: Unlock");

	data=NULL;

	return true;
}

D3DMFORMAT D3DMIndexBufferPeer::getD3DMFORMAT(IndexBuffer::IndexFormat format){
	D3DMFORMAT d3dFormat=(D3DMFORMAT)0;
	switch(format){
		case IndexBuffer::IndexFormat_UINT_8:
		case IndexBuffer::IndexFormat_UINT_16:
			d3dFormat=D3DMFMT_INDEX16;
		break;
		case IndexBuffer::IndexFormat_UINT_32:
			d3dFormat=D3DMFMT_INDEX32;
		break;
		default:
		break;
	}

	if(d3dFormat==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMIndexBufferPeer::getD3DMFORMAT: Invalid format");
		return (D3DMFORMAT)0;
	}

	return d3dFormat;
}

}
}
