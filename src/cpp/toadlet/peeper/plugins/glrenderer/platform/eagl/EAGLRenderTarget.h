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

#ifndef TOADLET_PEEPER_EAGLRENDERCONTEXT_H
#define TOADLET_PEEPER_EAGLRENDERCONTEXT_H

#include "../../GLIncludes.h"
#include "../../GLRenderTarget.h"
#include <toadlet/peeper/Visual.h>
#include <toadlet/egg/Logger.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

namespace toadlet{
namespace peeper{

// TODO: Switch this to extending an FBOSurfaceRenderTarget
class TOADLET_API EAGLRenderContext:public GLRenderTarget{
public:
	EAGLRenderContext();
	EAGLRenderContext(CAEAGLLayer *drawable,const Visual &visual,NSString *colorFormat=nil);
	virtual ~EAGLRenderContext();

	virtual RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	virtual bool createContext(CAEAGLLayer *drawable,const Visual &visual,NSString *colorFormat=nil);
	virtual bool destroyContext();

	virtual bool makeCurrent();
	virtual bool swap();
	
	virtual bool isPrimary() const{return true;}
	virtual bool isValid() const{return mContext!=nil && mRenderBuffer!=0;}
	virtual int getWidth() const;
	virtual int getHeight() const;

protected:
	CAEAGLLayer *mDrawable;
	EAGLContext *mContext;

	GLuint mRenderBuffer;
	GLuint mFrameBuffer;
	GLuint mDepthBuffer;
};

}
}

#endif

