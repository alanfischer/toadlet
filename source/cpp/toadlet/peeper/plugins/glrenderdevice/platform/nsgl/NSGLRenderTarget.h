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

#ifndef TOADLET_PEEPER_NSGLRENDERTARGET_H
#define TOADLET_PEEPER_NSGLRENDERTARGET_H

#include "../../GLIncludes.h"
#include "../../GLRenderTarget.h"
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/WindowRenderTargetFormat.h>

#import <Cocoa/Cocoa.h>

namespace toadlet{
namespace peeper{

class TOADLET_API NSGLRenderTarget:public GLRenderTarget{
public:
	NSGLRenderTarget();
	NSGLRenderTarget(NSView *view,WindowRenderTargetFormat *format,NSOpenGLPixelFormat *pixelFormat=nil);
	NSGLRenderTarget(NSOpenGLContext *context);
	virtual ~NSGLRenderTarget();
	void destroy();

	RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}
	
	bool createContext(NSView *view,WindowRenderTargetFormat *format,NSOpenGLPixelFormat *pixelFormat=nil);
	bool destroyContext();

	bool activate();
	bool deactivate();
	bool swap();
	void reset();
	bool activateAdditionalContext();
	void deactivateAdditionalContext();

	bool isPrimary() const{return true;}
	bool isValid() const{return mContext!=NULL;}
	int getHeight() const;
	int getWidth() const;
	inline NSOpenGLContext *getContext() const{return mContext;}

protected:
	NSView *mView;
	NSOpenGLPixelFormat *mPixelFormat;
	NSOpenGLContext *mContext;
	Collection<NSOpenGLContext*> mThreadContexts;
	Collection<int> mThreadIDs;
};

}
}

#endif

