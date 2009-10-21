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

#ifndef TOADLET_PEEPER_GLXPBUFFERRENDERTEXTUREPEER_H
#define TOADLET_PEEPER_GLXPBUFFERRENDERTEXTUREPEER_H

#include "../../GLTexturePeer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/RenderTexture.h>
#include "GLXRenderTargetPeer.h"

namespace toadlet{
namespace peeper{

class GLRenderer;
	
class GLXPBufferRenderTexturePeer:public GLXRenderTargetPeer,public GLTexturePeer{
public:
	static bool available(GLRenderer *renderer);

	GLXPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture);

	virtual ~GLXPBufferRenderTexturePeer();

	TexturePeer *castToTexturePeer(){return this;}
	RenderTargetPeer *castToRenderTargetPeer(){return this;}

	void makeCurrent();

	void swap();

	void createBuffer(GLRenderer *renderer);

	void destroyBuffer();

	int getWidth() const;
	int getHeight() const;

	bool isValid() const;

	inline GLXPbuffer getGLXPbuffer() const{return mPBuffer;}

protected:
	RenderTexture *mTexture;
	GLXPbuffer mPBuffer;
	int mWidth;
	int mHeight;
	bool mBound;
	bool mShare;
};

}
}

#endif

