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
#if 0
#ifndef TOADLET_PEEPER_D3D10SURFACE_H
#define TOADLET_PEEPER_D3D10SURFACE_H

#include "D3D10Includes.h"
#include <toadlet/peeper/Surface.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9Surface:public Surface{
public:
	TOADLET_SHARED_POINTERS(D3D9Surface);

	D3D9Surface(IDirect3DSurface9 *surface);
	virtual ~D3D9Surface();

	virtual Surface *getRootSurface(){return this;}

	virtual void destroy();

	// TODO: Implement these, or somehow remove them from the Surface requirements
	virtual int getUsageFlags() const{return 0;}
	virtual AccessType getAccessType() const{return AccessType_NO_ACCESS;}
	virtual int getDataSize() const{return 0;}

	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

	virtual uint8 *lock(AccessType accessType);
	virtual bool unlock();

	inline IDirect3DSurface9 *getSurface() const{return mSurface;}

protected:
	IDirect3DSurface9 *mSurface;
	int mWidth;
	int mHeight;

	friend D3D9Renderer;
};

}
}

#endif
#endif