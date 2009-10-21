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

package com.lightningtoads.toadlet.peeper;

#include <com/lightningtoads/toadlet/peeper/Types.h>

public class RenderTexture extends Texture implements RenderTarget{
	public RenderTexture(int format,int width,int height){
		super(Dimension.D2,format,width,height);

		mType=Type.RENDER;

		mSAddressMode=AddressMode.CLAMP_TO_EDGE;
		mTAddressMode=AddressMode.CLAMP_TO_EDGE;
		mRAddressMode=AddressMode.CLAMP_TO_EDGE;

		mMipFilter=Filter.NONE;
	}

	public RenderTexture(Renderer renderer,int format,int width,int height){
		super(Dimension.D2,format,width,height);

		mType=Type.RENDER;

		mSAddressMode=AddressMode.CLAMP_TO_EDGE;
		mTAddressMode=AddressMode.CLAMP_TO_EDGE;
		mRAddressMode=AddressMode.CLAMP_TO_EDGE;

		mMipFilter=Filter.NONE;

		mTexturePeer=renderer.createTexturePeer(this);
		//if(mTexturePeer!=null){
		//	mRenderTargetPeer=mTexturePeer.castToRenderTargetPeer();
		//}
	}

	public Texture castToTexture(){return this;}
	public RenderTarget castToRenderTarget(){return null;}//this;}

	public int getWidth(){
		//if(mRenderTargetPeer!=null){
		//	return RenderTarget.getWidth();
		//}
		//else{
			return mWidth;
		//}
	}

	public int getHeight(){
		//if(mRenderTargetPeer!=null){
		//	return RenderTarget.getHeight();
		//}
		//else{
			return mHeight;
		//}
	}

	public boolean hasDepthBuffer(){return true;}

	public void internal_setTexturePeer(Texture.Peer texturePeer){
		// TODO
	}

	public RenderTarget.Peer internal_getRenderTargetPeer(){
		// TODO
		return null;
	}
	public void internal_setRenderTargetPeer(RenderTarget.Peer renderTargetPeer){
		// TODO
	}
}