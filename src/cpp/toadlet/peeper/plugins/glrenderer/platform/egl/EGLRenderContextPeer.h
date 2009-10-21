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

#ifndef TOADLET_PEEPER_EGLRENDERCONTEXTPEER_H
#define TOADLET_PEEPER_EGLRENDERCONTEXTPEER_H

#include "EGLRenderTargetPeer.h"
#include <toadlet/peeper/Visual.h>

namespace toadlet{
namespace peeper{

class TOADLET_API EGLRenderContextPeer:public EGLRenderTargetPeer{
public:
	EGLRenderContextPeer();
	EGLRenderContextPeer(void *nativeSurface,const Visual &visual,bool pixmap=false);
	virtual ~EGLRenderContextPeer();

	virtual bool createContext(void *nativeSurface,const Visual &visual,bool pixmap=false);

	virtual bool destroyContext();

	virtual int getWidth() const;
	
	virtual int getHeight() const;
	
	virtual void swap();

	inline bool isValid() const{return mContext!=EGL_NO_CONTEXT;}

protected:
	EGLConfig mConfig;
	bool mPixmap;
};

}
}

#endif
