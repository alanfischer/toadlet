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

#ifndef TOADLET_PEEPER_EGLPBUFFERRENDERTEXTUREPEER_H
#define TOADLET_PEEPER_EGLPBUFFERRENDERTEXTUREPEER_H

#include "EGLRenderTargetPeer.h"
#include "../../GLTexturePeer.h"
#include <toadlet/peeper/RenderTexture.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class EGLPBufferRenderTexturePeer:public EGLRenderTargetPeer,public GLTexturePeer{
public:
	static bool available(GLRenderer *renderer);

	EGLPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture);

	virtual ~EGLPBufferRenderTexturePeer();

	void makeCurrent();

	void swap();

	TexturePeer *castToTexturePeer(){return this;}
	RenderTargetPeer *castToRenderTargetPeer(){return this;}

	bool createBuffer();

	bool destroyBuffer();

	int getWidth() const;
	int getHeight() const;

	bool isValid() const{return mContext!=NULL && mSurface!=NULL;}

protected:
	RenderTexture *mTexture;
	int mWidth;
	int mHeight;
	bool mBound;
};

}
}

#endif

