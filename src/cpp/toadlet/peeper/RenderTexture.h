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

#ifndef TOADLET_PEEPER_RENDERTEXTURE_H
#define TOADLET_PEEPER_RENDERTEXTURE_H

#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/RenderTarget.h>

namespace toadlet{
namespace peeper{

class TOADLET_API RenderTexture:public Texture,public RenderTarget{
public:
	typedef egg::SharedPointer<RenderTexture,Texture> Ptr;
	typedef egg::WeakPointer<RenderTexture,Texture,egg::DefaultSharedSemantics> WeakPtr;

	RenderTexture(int format,int width,int height);
	RenderTexture(Renderer *renderer,int format,int width,int height);

	virtual ~RenderTexture();

	virtual Texture *castToTexture(){return this;}
	virtual RenderTarget *castToRenderTarget(){return this;}

	virtual int getWidth() const;
	virtual int getHeight() const;
	virtual bool hasDepthBuffer() const{return true;}

	virtual void internal_setTexturePeer(TexturePeer *texturePeer,bool own);
	virtual void internal_setRenderTargetPeer(RenderTargetPeer *renderTargetPeer,bool own);

//protected:
//	bool mDepthBuffer;
};

}
}

#endif
