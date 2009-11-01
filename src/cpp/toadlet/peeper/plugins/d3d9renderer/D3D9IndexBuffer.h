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

#ifndef TOADLET_PEEPER_D3D9INDEXBUFFER_H
#define TOADLET_PEEPER_D3D9INDEXBUFFER_H

#include "D3D9Includes.h"
#include <toadlet/peeper/IndexBuffer.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9IndexBuffer:public IndexBuffer{
public:
	D3D9IndexBuffer(D3D9Renderer *renderer);
	virtual ~D3D9IndexBuffer();

	IndexBuffer *getRootIndexBuffer(){return this;}

	virtual bool create(int usageFlags,AccessType accessType,IndexFormat indexFormat,int size);
	virtual bool destroy();

	virtual int getUsageFlags() const{return mUsageFlags;}
	virtual AccessType getAccessType() const{return mAccessType;}
	virtual int getDataSize() const{return mDataSize;}
	virtual IndexFormat getIndexFormat(){return mIndexFormat;}
	virtual int getSize() const{return mSize;}

	virtual uint8 *lock(AccessType accessType);
	virtual bool unlock();

protected:
	static D3DFORMAT getD3DFORMAT(IndexFormat format);

	D3D9Renderer *mRenderer;

	int mUsageFlags;
	AccessType mAccessType;
	IndexFormat mIndexFormat;
	int mSize;
	int mDataSize;

	IDirect3DIndexBuffer9 *mIndexBuffer;

	uint8 *mData;
	AccessType mLockType;

	friend D3D9Renderer;
};

}
}

#endif
