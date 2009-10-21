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

#ifndef TOADLET_PEEPER_D3DMRENDERTEXTUREPEER_H
#define TOADLET_PEEPER_D3DMRENDERTEXTUREPEER_H

#include "../../RenderTexture.h"
#include "D3DMRenderTargetPeer.h"
#include "D3DMTexturePeer.h"

namespace toadlet{
namespace peeper{

class D3DMRenderer;

class TOADLET_API D3DMRenderTexturePeer:public D3DMRenderTargetPeer,public D3DMTexturePeer{
public:
	D3DMRenderTexturePeer(D3DMRenderer *renderer,RenderTexture *renderTexture);

	virtual ~D3DMRenderTexturePeer();

	int getWidth() const;
	int getHeight() const;

	bool isValid() const;

	void makeCurrent(IDirect3DMobileDevice *device);

	IDirect3DMobileSurface *colorSurface;
	IDirect3DMobileSurface *depthSurface;	
};

}
}

#endif