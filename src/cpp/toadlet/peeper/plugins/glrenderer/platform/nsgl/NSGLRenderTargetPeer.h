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

#ifndef TOADLET_PEEPER_NSGLRENDERTARGETPEER_H
#define TOADLET_PEEPER_NSGLRENDERTARGETPEER_H

#include "../../GLIncludes.h"
#include "../../GLRenderTargetPeer.h"
#include <toadlet/peeper/Visual.h>
#include <toadlet/egg/Error.h>

#import <Cocoa/Cocoa.h>

namespace toadlet{
namespace peeper{

class TOADLET_API NSGLRenderTargetPeer:public GLRenderTargetPeer{
public:
	NSGLRenderTargetPeer();
	NSGLRenderTargetPeer(NSView *view,const Visual &visual,NSOpenGLPixelFormat *pixelFormat=nil);
	NSGLRenderTargetPeer(NSOpenGLContext *context);
	virtual ~NSGLRenderTargetPeer();

	virtual bool createContext(NSView *view,const Visual &visual,NSOpenGLPixelFormat *pixelFormat=nil);

	virtual void destroyContext();

	virtual void makeCurrent();

	virtual int getHeight() const;

	virtual int getWidth() const;

	virtual void swap();

	inline bool isValid() const{return mContext!=NULL;}

	inline NSOpenGLContext *getContext() const{return mContext;}

protected:
	NSView *mView;
	NSOpenGLPixelFormat *mPixelFormat;
	NSOpenGLContext *mContext;
};

}
}

#endif

