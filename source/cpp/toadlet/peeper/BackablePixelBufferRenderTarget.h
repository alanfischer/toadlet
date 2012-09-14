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

#ifndef TOADLET_PEEPER_BACKABLEPIXELBUFFERRENDERTARGET_H
#define TOADLET_PEEPER_BACKABLEPIXELBUFFERRENDERTARGET_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/PixelBufferRenderTarget.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackablePixelBufferRenderTarget:public BaseResource,public PixelBufferRenderTarget{
public:
	TOADLET_RESOURCE(BackablePixelBufferRenderTarget,PixelBufferRenderTarget);

	BackablePixelBufferRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return mBack!=NULL?mBack->getRootRenderTarget():NULL;}
	virtual PixelBufferRenderTarget *getRootPixelBufferRenderTarget(){return mBack!=NULL?mBack->getRootPixelBufferRenderTarget():NULL;}

	virtual bool isPrimary() const{return mBack!=NULL?mBack->isPrimary():false;}
	virtual bool isValid() const{return mBack!=NULL?mBack->isValid():false;}
	virtual int getWidth() const{return mBack!=NULL?mBack->getWidth():0;}
	virtual int getHeight() const{return mBack!=NULL?mBack->getHeight():0;}

	virtual bool create();
	virtual void destroy();

	virtual void resetCreate(){if(mBack!=NULL){mBack->resetCreate();}}
	virtual void resetDestroy(){if(mBack!=NULL){mBack->resetDestroy();}}

	virtual bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	virtual bool remove(PixelBuffer::ptr buffer);

	virtual void setBack(PixelBufferRenderTarget::ptr back);
	virtual PixelBufferRenderTarget::ptr getBack(){return mBack;}
	
protected:
	PixelBufferRenderTarget::ptr mBack;
	Collection<PixelBuffer::ptr> mBuffers;
	Collection<Attachment> mBufferAttachments;
};

}
}

#endif
