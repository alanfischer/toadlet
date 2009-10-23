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

#ifndef TOADLET_PEEPER_SURFACERENDERTARGET_H
#define TOADLET_PEEPER_SURFACERENDERTARGET_H

#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/Surface.h>

namespace toadlet{
namespace peeper{

class SurfaceRenderTarget:public RenderTarget{
public:
	TOADLET_SHARED_POINTERS(SurfaceRenderTarget,RenderTarget);

	enum Attachment{
		Attachment_DEPTH_STENCIL,
		Attachment_COLOR_0,
		Attachment_COLOR_1,
		Attachment_COLOR_2,
		Attachment_COLOR_3,
	};

	virtual ~SurfaceRenderTarget(){}

	virtual bool create()=0;
	virtual bool destroy()=0;
	
	virtual bool attach(Surface::ptr surface,Attachment attachment)=0;
	virtual bool remove(Surface::ptr surface)=0;
};

}
}

#endif
