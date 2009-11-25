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

#ifndef TOADLET_PEEPER_GLXPBUFFERSURFACERENDERTARGET_H
#define TOADLET_PEEPER_GLXPBUFFERSURFACERENDERTARGET_H

#include "GLXRenderTarget.h"
#include "../../GLTexture.h"
#include <toadlet/peeper/SurfaceRenderTarget.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class GLXPBufferSurfaceRenderTarget:public GLXRenderTarget,public SurfaceRenderTarget{
public:
	static bool available(GLRenderer *renderer);

	GLXPBufferSurfaceRenderTarget(GLRenderer *renderer);
	virtual ~GLXPBufferSurfaceRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	virtual bool create();
	virtual bool destroy();
	virtual bool compile();

	virtual bool makeCurrent();
	virtual bool swap();

	virtual bool attach(Surface::ptr surface,Attachment attachment);
	virtual bool remove(Surface::ptr surface);

	virtual bool isPrimary() const{return false;}
	virtual bool isValid() const{return mPBuffer!=0;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	inline GLXPbuffer getGLXPbuffer() const{return mPBuffer;}

protected:
	bool createBuffer();
	bool destroyBuffer();

	GLRenderer *mRenderer;
	GLTexture *mTexture;
	GLXPbuffer mPBuffer;
	int mWidth;
	int mHeight;
	bool mBound;
	bool mInitialized;
};

}
}

#endif

