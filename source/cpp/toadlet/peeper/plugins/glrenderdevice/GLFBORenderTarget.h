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
#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/PixelBufferRenderTarget.h>

#if defined(TOADLET_HAS_GLFBOS)

namespace toadlet{
namespace peeper{

class GLRenderDevice;

/// @todo: Make the D3D9PixelBufferRenderTarget & the GLFBORenderTarget more strict about sizes
class TOADLET_API GLFBORenderTarget:public GLRenderTarget,public PixelBufferRenderTarget{
public:
	TOADLET_RESOURCE(GLFBORenderTarget,PixelBufferRenderTarget);

	static bool available(GLRenderDevice *renderDevice);

	GLFBORenderTarget(GLRenderDevice *renderDevice);

	RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}
	PixelBufferRenderTarget *getRootPixelBufferRenderTarget(){return this;}

	bool create();
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	bool activate();
	bool deactivate();
	bool swap(){return false;}
	bool share(GLRenderTarget *target){return true;}
	bool activateAdditionalContext(){return false;}
	void deactivateAdditionalContext(){}

	bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	bool remove(PixelBuffer::ptr buffer);
	bool compile();

	bool isPrimary() const{return false;}
	bool isValid() const{return mHandle!=0;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}

	AdaptorInfo *getAdaptorInfo() const{return GLRenderTarget::getAdaptorInfo();}

	GLuint getHandle(){return mHandle;}
	
	static GLenum getGLAttachment(Attachment attachment);
	static const char *getFBOMessage(GLenum status);

protected:
	GLRenderDevice *mDevice;
	int mWidth;
	int mHeight;
	GLuint mHandle;
	bool mNeedsCompile;
	Collection<PixelBuffer::ptr> mBuffers;
	Collection<Attachment> mBufferAttachments;
	PixelBuffer::ptr mOwnedDepthBuffer;
};

}
}

#endif

#endif
