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

#ifndef TOADLET_PEEPER_GLRENDERTARGET_H
#define TOADLET_PEEPER_GLRENDERTARGET_H

#include "GLIncludes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/RenderTarget.h>

namespace toadlet{
namespace peeper{

class GLRenderTarget:public BaseResource,public RenderTarget{
public:
	TOADLET_RESOURCE(GLRenderTarget,RenderTarget);

	virtual ~GLRenderTarget(){}

	virtual RenderTarget *getRootRenderTarget(){return this;}

	virtual void destroy(){}

	virtual bool isPrimary() const{return 0;}
	virtual bool isValid() const{return 0;}
	virtual int getWidth() const{return 0;}
	virtual int getHeight() const{return 0;}

	virtual bool swap(){return false;}
	virtual void reset(){} // Mostly unused except for NSGLRenderTarget
	virtual bool activate(){return false;}
	virtual bool deactivate(){return false;}
	virtual bool activateAdditionalContext(){return false;}
	virtual void deactivateAdditionalContext(){}
};

}
}

#endif
