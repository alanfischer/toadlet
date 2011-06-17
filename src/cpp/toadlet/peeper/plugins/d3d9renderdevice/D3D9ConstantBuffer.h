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

#ifndef TOADLET_PEEPER_D3D9CONSTANTBUFFER_H
#define TOADLET_PEEPER_D3D9CONSTANTBUFFER_H

#include "D3D9Includes.h"
#include <toadlet/peeper/ConstantBuffer.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9ConstantBuffer:public ConstantBuffer{
public:
	D3D9ConstantBuffer(D3D9RenderDevice *renderDevice);
	virtual ~D3D9ConstantBuffer();

	ConstantBuffer *getRootConstantBuffer(){return this;}

	void setBufferDestroyedListener(BufferDestroyedListener *listener){mListener=listener;}

	bool create(int usage,int access,int size);
	void destroy();

	void resetCreate();
	void resetDestroy();

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	int getDataSize() const{return mDataSize;}
	int getSize() const{return mSize;}

	uint8 *lock(int lockAccess){return NULL;}
	bool unlock(){return false;}

	void setConstant(int location,tbyte *value)=0;

protected:
	bool createContext(bool restore);
	bool destroyContext(bool backup);

	D3D9RenderDevice *mDevice;

	BufferDestroyedListener *mListener;
	int mUsage;
	int mAccess;
	int mSize;
	int mDataSize;
	
	egg::Collection<int> mConstantLocations;
	egg::Collection<int> mConstantSizes;

	friend D3D9RenderDevice;
};

}
}

#endif
