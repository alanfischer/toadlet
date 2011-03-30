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

#ifndef TOADLET_PEEPER_EGLPBUFFERRENDERTARGET_H
#define TOADLET_PEEPER_EGLPBUFFERRENDERTARGET_H

#include "EGLRenderTarget.h"
#include "../../GLTexture.h"
#include <toadlet/peeper/SurfaceRenderTarget.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class EGLPBufferRenderTarget:public EGLRenderTarget,public SurfaceRenderTarget{
public:
	static bool available(GLRenderer *renderer);

	EGLPBufferRenderTarget(GLRenderer *renderer);
	virtual ~EGLPBufferRenderTarget();

	RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	void setRenderTargetDestroyedListener(RenderTargetDestroyedListener *listener){mListener=listener;}

	bool create();
	void destroy();
	bool compile();

	bool makeCurrent();
	bool swap();

	bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	bool remove(PixelBuffer::ptr buffer);

	bool isPrimary() const{return false;}
	bool isValid() const{return mContext!=NULL && mSurface!=NULL;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}

protected:
	bool createBuffer();
	bool destroyBuffer();
	void bind();
	void unbind();

	GLRenderer *mRenderer;
	RenderTargetDestroyedListener *mListener;
	bool mCopy;
	bool mSeparateContext;
	GLTexture *mTexture;
	int mWidth;
	int mHeight;
	bool mBound;
	bool mInitialized;
};

}
}

#endif

