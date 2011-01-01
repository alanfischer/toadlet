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

#ifndef TOADLET_PEEPER_GLFBORENDERTARGET_H
#define TOADLET_PEEPER_GLFBORENDERTARGET_H

#include "GLRenderTarget.h"
#include "GLFBOPixelBuffer.h"
#include <toadlet/peeper/PixelBufferRenderTarget.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

/// @todo: Make the D3D9PixelBufferRenderTarget & the GLFBORenderTarget more strict about sizes
class TOADLET_API GLFBORenderTarget:public GLRenderTarget,public PixelBufferRenderTarget{
public:
	static bool available(GLRenderer *renderer);

	GLFBORenderTarget(GLRenderer *renderer);
	virtual ~GLFBORenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	virtual bool create();
	virtual void destroy();

	virtual bool swap();
	virtual bool activate();
	virtual bool deactivate();
	virtual bool share(GLRenderTarget *target){return true;}
	virtual bool activateAdditionalContext(){return false;}
	virtual void deactivateAdditionalContext(){}

	virtual bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	virtual bool remove(PixelBuffer::ptr buffer);
	virtual bool compile();

	virtual bool isPrimary() const{return false;}
	virtual bool isValid() const{return mHandle!=0;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

protected:
	static GLenum getGLAttachment(Attachment attachment);
	static const char *getFBOMessage(GLenum status);

	GLRenderer *mRenderer;
	int mWidth;
	int mHeight;
	GLuint mHandle;
	bool mNeedsCompile;
	egg::Collection<PixelBuffer::ptr> mBuffers;
	egg::Collection<Attachment> mBufferAttachments;
	PixelBuffer::ptr mDepthBuffer;
};

}
}

#endif
