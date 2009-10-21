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

#ifndef TOADLET_PEEPER_D3D9VERTEXBUFFERPEER_H
#define TOADLET_PEEPER_D3D9VERTEXBUFFERPEER_H

#include "D3D9Includes.h"
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9VertexBufferPeer:public BufferPeer{
public:
	D3D9VertexBufferPeer(D3D9Renderer *renderer,VertexBuffer *buffer);

	virtual ~D3D9VertexBufferPeer();

	uint8 *lock(Buffer::LockType lockType);

	bool unlock();

	bool supportsRead(){return true;}

	inline bool isValid() const{return vertexBuffer!=NULL;}

	static DWORD getFVF(VertexBuffer *buffer,egg::Collection<VertexElement> *colorElements);

	Buffer::UsageType usageType;
	IDirect3DVertexBuffer9 *vertexBuffer;
	DWORD fvf;
	uint8 *data;

	Buffer::LockType lockType;
	int size;
	short vertexSize;
	egg::Collection<VertexElement> colorElements;
};

}
}

#endif
