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

#ifndef TOADLET_PEEPER_GLFBOSURFACERENDERTARGET_H
#define TOADLET_PEEPER_GLFBOSURFACERENDERTARGET_H

#include "GLRenderTarget.h"
#include "GLFBORenderbufferSurface.h"
#include <toadlet/peeper/SurfaceRenderTarget.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class TOADLET_API GLFBOSurfaceRenderTarget:public GLRenderTarget,public SurfaceRenderTarget{
public:
	static bool available(GLRenderer *renderer);

	GLFBOSurfaceRenderTarget(GLRenderer *renderer);
	virtual ~GLFBOSurfaceRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	virtual bool create();
	virtual bool destroy();

	virtual bool current();
	virtual bool swap();

	virtual Surface::ptr createBufferSurface(int format,int width,int height);
	virtual bool attach(Surface::ptr surface,Attachment attachment);
	virtual bool remove(Surface::ptr surface);
	
	virtual bool isPrimary() const{return false;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

protected:
	static GLenum getGLAttachment(Attachment attachment);
	static const char *getFBOMessage(GLenum status);

	GLRenderer *mRenderer;
	int mWidth;
	int mHeight;
	GLuint mHandle;
	egg::Collection<Surface::ptr> mSurfaces;
	egg::Collection<Attachment> mSurfaceAttachments;
	egg::Collection<GLFBORenderbufferSurface::ptr> mOwnedSurfaces;
};

}
}

#endif
