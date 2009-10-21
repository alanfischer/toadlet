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

#ifndef TOADLET_PEEPER_D3DMINDEXBUFFERPEER_H
#define TOADLET_PEEPER_D3DMINDEXBUFFERPEER_H

#include "D3DMIncludes.h"
#include <toadlet/peeper/IndexBuffer.h>

namespace toadlet{
namespace peeper{

class D3DMRenderer;

class TOADLET_API D3DMIndexBufferPeer:public BufferPeer{
public:
	D3DMIndexBufferPeer(D3DMRenderer *renderer,IndexBuffer *buffer);

	virtual ~D3DMIndexBufferPeer();

	uint8 *lock(Buffer::LockType lockType);

	bool unlock();

	bool supportsRead(){return true;}

	inline bool isValid() const{return indexBuffer!=NULL;}

	static D3DMFORMAT getD3DMFORMAT(IndexBuffer::IndexFormat format);

	Buffer::UsageType usageType;
	IDirect3DMobileIndexBuffer *indexBuffer;
	uint8 *data;

	Buffer::LockType lockType;
	int size;
	IndexBuffer::IndexFormat format;
};

}
}

#endif
