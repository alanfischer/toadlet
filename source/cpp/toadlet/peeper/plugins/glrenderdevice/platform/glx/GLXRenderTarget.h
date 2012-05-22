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

#ifndef TOADLET_PEEPER_GLXRENDERTARGET_H
#define TOADLET_PEEPER_GLXRENDERTARGET_H

#include "../../GLIncludes.h"
#include "../../GLRenderTarget.h"
#include <X11/Xlib.h>

namespace toadlet{
namespace peeper{

class GLXRenderTarget:public GLRenderTarget{
public:
	GLXRenderTarget();
	virtual ~GLXRenderTarget(){}

	virtual bool activate();
	virtual bool deactivate();

	inline Display *getDisplay() const{return mDisplay;}
	inline GLXDrawable getGLXDrawable() const{return mDrawable;}
	inline GLXContext getGLXContext() const{return mContext;}

protected:
	Display *mDisplay;
	GLXDrawable mDrawable;
	GLXContext mContext;
};

}
}

#endif
