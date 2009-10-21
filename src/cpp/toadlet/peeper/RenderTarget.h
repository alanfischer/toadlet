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

#ifndef TOADLET_PEEPER_RENDERTARGET_H
#define TOADLET_PEEPER_RENDERTARGET_H

#include <toadlet/peeper/Types.h>
#include <toadlet/egg/SharedPointer.h>

namespace toadlet{
namespace peeper{

class TexturePeer;

class RenderTargetPeer{
public:
	virtual ~RenderTargetPeer(){}

	virtual TexturePeer *castToTexturePeer(){return NULL;}

	virtual bool isValid() const=0;

	virtual int getWidth() const=0;
	virtual int getHeight() const=0;
};

class Texture;

class TOADLET_API RenderTarget{
public:
	RenderTarget();

	virtual ~RenderTarget();

	virtual Texture *castToTexture(){return NULL;}

	virtual int getWidth() const{return mRenderTargetPeer!=NULL?mRenderTargetPeer->getWidth():0;}
	virtual int getHeight() const{return mRenderTargetPeer!=NULL?mRenderTargetPeer->getHeight():0;}

	virtual void internal_setRenderTargetPeer(RenderTargetPeer *renderTargetPeer,bool own);
	inline RenderTargetPeer *internal_getRenderTargetPeer() const{return mRenderTargetPeer;}
	inline bool internal_ownsRenderTargetPeer() const{return mOwnsRenderTargetPeer;}

protected:
	RenderTargetPeer *mRenderTargetPeer;
	bool mOwnsRenderTargetPeer;
};

}
}

#endif
