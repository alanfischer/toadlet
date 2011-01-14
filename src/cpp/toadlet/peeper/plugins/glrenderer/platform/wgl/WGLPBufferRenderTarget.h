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

#ifndef TOADLET_PEEPER_WGLPBUFFERRENDERTARGET_H
#define TOADLET_PEEPER_WGLPBUFFERRENDERTARGET_H

#include "WGLRenderTarget.h"
#include "../../GLTexture.h"
#include <toadlet/peeper/PixelBufferRenderTarget.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class WGLPBufferRenderTarget:public WGLRenderTarget,public PixelBufferRenderTarget{
public:
	static bool available(GLRenderer *renderer);

	WGLPBufferRenderTarget(GLRenderer *renderer);
	virtual ~WGLPBufferRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	virtual bool create();
	virtual void destroy();
	virtual bool compile();

	virtual bool activate();
	virtual bool deactivate(){return WGLRenderTarget::deactivate();}
	virtual bool swap();
	virtual bool activateAdditionalContext(){return false;}
	virtual void deactivateAdditionalContext(){}

	virtual bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	virtual bool remove(PixelBuffer::ptr buffer);

	virtual bool isPrimary() const{return false;}
	virtual bool isValid() const{return mGLRC!=NULL && mPBuffer!=NULL;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	inline HPBUFFERARB getHPBUFFER() const{return mPBuffer;}

protected:
	bool createBuffer();
	bool destroyBuffer();
	void bind();
	void unbind();

	GLRenderer *mRenderer;
	bool mCopy;
	GLTexture *mTexture;
	HPBUFFERARB mPBuffer;
	int mWidth;
	int mHeight;
	bool mBound;
	bool mInitialized;
};

}
}

#endif
