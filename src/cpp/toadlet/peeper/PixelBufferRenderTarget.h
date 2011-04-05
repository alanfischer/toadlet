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

#ifndef TOADLET_PEEPER_PIXELBUFFERRENDERTARGET_H
#define TOADLET_PEEPER_PIXELBUFFERRENDERTARGET_H

#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/PixelBuffer.h>
#include <toadlet/peeper/RenderTargetDestroyedListener.h>

namespace toadlet{
namespace peeper{

class PixelBufferRenderTarget:public RenderTarget{
public:
	TOADLET_SHARED_POINTERS(PixelBufferRenderTarget);

	enum Attachment{
		Attachment_DEPTH_STENCIL,
		Attachment_COLOR_0,
		Attachment_COLOR_1,
		Attachment_COLOR_2,
		Attachment_COLOR_3,
	};

	virtual ~PixelBufferRenderTarget(){}

	virtual PixelBufferRenderTarget *getRootPixelBufferRenderTarget()=0;

	virtual void setRenderTargetDestroyedListener(RenderTargetDestroyedListener *listener)=0;

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual bool attach(PixelBuffer::ptr buffer,Attachment attachment)=0;
	virtual bool remove(PixelBuffer::ptr buffer)=0;
};

}
}

#endif
