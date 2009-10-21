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

#ifndef TOADLET_PEEPER_D3D9TEXTUREPEER_H
#define TOADLET_PEEPER_D3D9TEXTUREPEER_H

#include "D3D9Includes.h"
#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9TexturePeer:public TexturePeer{
public:
	D3D9TexturePeer(D3D9Renderer *renderer,Texture *texture);

	virtual ~D3D9TexturePeer();

	bool isD3DFORMATValid(IDirect3D9 *d3d,D3DFORMAT textureFormat,D3DFORMAT adapterFormat);

	static D3DFORMAT getD3DFORMAT(int textureFormat);
	static DWORD getD3DTADDRESS(Texture::AddressMode addressMode);
	static DWORD getD3DTEXF(Texture::Filter filter);

	IDirect3DBaseTexture9 *baseTexture;
};

}
}

#endif
