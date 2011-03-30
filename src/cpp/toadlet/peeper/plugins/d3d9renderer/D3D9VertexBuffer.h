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

#ifndef TOADLET_PEEPER_D3D9VERTEXBUFFER_H
#define TOADLET_PEEPER_D3D9VERTEXBUFFER_H

#include "D3D9Includes.h"
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9VertexBuffer:public VertexBuffer{
public:
	D3D9VertexBuffer(D3D9Renderer *renderer);
	virtual ~D3D9VertexBuffer();

	VertexBuffer *getRootVertexBuffer(){return this;}

	void setBufferDestroyedListener(BufferDestroyedListener *listener){mListener=listener;}

	bool create(int usage,int access,VertexFormat::ptr vertexFormat,int size);
	void destroy();

	void resetCreate();
	void resetDestroy();

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	int getDataSize() const{return mDataSize;}
	VertexFormat::ptr getVertexFormat() const{return mVertexFormat;}
	int getSize() const{return mSize;}

	uint8 *lock(int lockAccess);
	bool unlock();

	bool needsReset();

protected:
	bool createContext(bool restore);
	bool destroyContext(bool backup);

	D3D9Renderer *mRenderer;

	BufferDestroyedListener *mListener;
	int mUsage;
	int mAccess;
	int mSize;
	VertexFormat::ptr mVertexFormat;
	short mVertexSize;
	int mDataSize;

	DWORD mD3DUsage;
	D3DPOOL mD3DPool;
	IDirect3DVertexBuffer9 *mVertexBuffer;
	int mLockAccess;
	uint8 *mData;
	uint8 *mBackingData;

	friend D3D9Renderer;
};

}
}

#endif
