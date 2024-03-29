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

#ifndef TOADLET_PEEPER_GLXWINDOWRENDERTARGET_H
#define TOADLET_PEEPER_GLXWINDOWRENDERTARGET_H

#include "GLXRenderTarget.h"
#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/WindowRenderTargetFormat.h>

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_GLXWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format,RenderTarget *shareTarget);

class TOADLET_API GLXWindowRenderTarget:public GLXRenderTarget{
public:
	GLXWindowRenderTarget();
	GLXWindowRenderTarget(Display *display,Window window,WindowRenderTargetFormat *format,RenderTarget *shareTarget);
	void destroy();

	RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	bool createContext(Display *display,Window window,WindowRenderTargetFormat *format,RenderTarget *shareTarget);
	bool destroyContext();
	bool activateAdditionalContext();
	void deactivateAdditionalContext();
	
	bool swap();

	bool isPrimary() const{return true;}
	bool isValid() const{return mContext!=0;}
	int getWidth() const;
	int getHeight() const;

protected:
	bool mOwnsDisplay;
	Collection<GLXContext> mThreadContexts;
	Collection<int> mThreadIDs;
};

}
}

#endif
