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

#ifndef TOADLET_PEEPER_GLXPBUFFERRENDERTARGET_H
#define TOADLET_PEEPER_GLXPBUFFERRENDERTARGET_H

#include "GLXRenderTarget.h"
#include "../../GLTexture.h"
#include <toadlet/peeper/PixelBufferRenderTarget.h>

namespace toadlet{
namespace peeper{

class GLRenderDevice;

class GLXPBufferRenderTarget:protected BaseResource,public GLXRenderTarget,public PixelBufferRenderTarget{
public:
	TOADLET_RESOURCE(GLXPBufferRenderTarget,PixelBufferRenderTarget);

	static bool available(GLRenderDevice *renderDevice);

	GLXPBufferRenderTarget(GLRenderDevice *renderDevice);
	virtual ~GLXPBufferRenderTarget();

	RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}
	PixelBufferRenderTarget *getRootPixelBufferRenderTarget(){return this;}

	bool create();
	void destroy();
	bool compile();

	void resetCreate(){}
	void resetDestroy(){}

	bool activate();
	bool deactivate();
	bool swap(){return false;}
	bool activateAdditionalContext(){return false;}
	void deactivateAdditionalContext(){}

	bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	bool remove(PixelBuffer::ptr buffer);

	bool isPrimary() const{return false;}
	bool isValid() const{return mPBuffer!=0;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}
	inline GLXPbuffer getGLXPbuffer() const{return mPBuffer;}

protected:
	bool createBuffer();
	bool destroyBuffer();
	void bind();
	void unbind();

	GLRenderDevice *mDevice;
	RenderTargetDestroyedListener *mListener;
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

