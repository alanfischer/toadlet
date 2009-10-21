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

#ifndef TOADLET_PEEPER_D3D9RENDERCONTEXTPEER_H
#define TOADLET_PEEPER_D3D9RENDERCONTEXTPEER_H

#include "D3D9RenderTargetPeer.h"

namespace toadlet{
namespace peeper{

class TOADLET_API D3D9RenderContextPeer:public D3D9RenderTargetPeer{
public:
	D3D9RenderContextPeer():D3D9RenderTargetPeer(){}
	virtual ~D3D9RenderContextPeer(){}

	virtual void reset()=0;

	virtual IDirect3DDevice9 *getDirect3DDevice9() const=0;
};

}
}

#endif

