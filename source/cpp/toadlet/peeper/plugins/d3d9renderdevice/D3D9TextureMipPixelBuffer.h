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

#ifndef TOADLET_PEEPER_D3D9TEXTUREMIPPIXELBUFFER_H
#define TOADLET_PEEPER_D3D9TEXTUREMIPPIXELBUFFER_H

#include "D3D9PixelBuffer.h"
#include "D3D9Texture.h"

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9TextureMipPixelBuffer:public D3D9PixelBuffer{
public:
	TOADLET_SPTR(D3D9TextureMipPixelBuffer);

	D3D9TextureMipPixelBuffer(D3D9Texture *texture,int level,int cubeSide);
	virtual ~D3D9TextureMipPixelBuffer();

	bool create(int usage,int access,TextureFormat::ptr format);
	void destroy();

	void resetCreate();
	void resetDestroy();

protected:
	D3D9Texture *mTexture;
	int mLevel;
	int mCubeSide;
};

}
}

#endif
