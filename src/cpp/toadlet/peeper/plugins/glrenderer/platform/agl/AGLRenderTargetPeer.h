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

#ifndef TOADLET_PEEPER_AGLRENDERTARGETPEER_H
#define TOADLET_PEEPER_AGLRENDERTARGETPEER_H

#include "../../GLRenderTargetPeer.h"
#include <toadlet/egg/Logger.h>
#include <Carbon/Carbon.h>

#if defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_AGLERROR(function) \
		{ int error=aglGetError(); \
		if(error!=GL_NO_ERROR) \
			toadlet::egg::Logger::log(toadlet::egg::Categories::TOADLET_PEEPER,toadlet::egg::Logger::Level_ALERT, \
				toadlet::egg::String("AGL Error in ") + function + ": error=" + aglErrorString(error)); }
#else
	#define TOADLET_CHECK_AGLERROR(function)
#endif

namespace toadlet{
namespace peeper{

class AGLRenderTargetPeer:public GLRenderTargetPeer{
public:
	AGLRenderTargetPeer():GLRenderTargetPeer(){
		mContext=NULL;
	}

	virtual ~AGLRenderTargetPeer(){}

	void makeCurrent(){
		GLboolean result=aglSetCurrentContext(mContext);
		if(result==GL_FALSE){
			egg::Logger::log(egg::Categories::TOADLET_PEEPER,egg::Logger::Level_WARNING,
				"aglSetCurrentContext failed");
		}
	}

	inline AGLContext getAGLContext() const{return mContext;}

protected:
	AGLContext mContext;
};

}
}

#endif
