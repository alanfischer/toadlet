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

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.egg.image.ImageDefinitions;

public class Texture implements Resource, ImageDefinitions{
	public interface Peer{
		public void destroy();
	
		public RenderTarget.Peer castToRenderTargetPeer();
	}

	public enum Type{
		UNKNOWN,
		NORMAL,
		RENDER,
		ANIMATED,
	}

	public enum AddressMode{
		REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRRORED_REPEAT,
	}

	public enum Filter{
		NONE,
		NEAREST,
		LINEAR,
	}

	public Texture(){}

	public Texture(android.graphics.Bitmap image){
		setImage(image);
	}
	
	public Texture(Renderer renderer,android.graphics.Bitmap image){
		setImage(image);

		mTexturePeer=renderer.createTexturePeer(this);
	}

	public Texture(Dimension dimension,int format,int width){init(null,dimension,format,width,1,1);}
	public Texture(Dimension dimension,int format,int width,int height){init(null,dimension,format,width,height,1);}
	public Texture(Dimension dimension,int format,int width,int height,int depth){init(null,dimension,format,width,height,depth);}
	public Texture(Renderer renderer,Dimension dimension,int format,int width){init(renderer,dimension,format,width,1,1);}
	public Texture(Renderer renderer,Dimension dimension,int format,int width,int height){init(renderer,dimension,format,width,height,1);}
	public Texture(Renderer renderer,Dimension dimension,int format,int width,int height,int depth){init(renderer,dimension,format,width,height,depth);}

	private void init(Renderer renderer,Dimension dimension,int format,int width,int height,int depth){
		mDimension=dimension;
		mFormat=format;
		mWidth=width;
		mHeight=height;
		mDepth=depth;

		if(renderer!=null){
			mTexturePeer=renderer.createTexturePeer(this);
		}
	}

	public void destroy(){
		if(mTexturePeer!=null){
			mTexturePeer.destroy();
		}
	}
	
	public RenderTarget castToRenderTarget(){return null;}

	public Dimension getDimension(){return mDimension;}
	public int getFormat(){return mFormat;}
	public Type getType(){return mType;}

	public int getWidth(){return mWidth;}
	public int getHeight(){return mHeight;}
	public int getDepth(){return mDepth;}

	public void setAutoGenerateMipMaps(boolean mipmaps){mAutoGenerateMipMaps=mipmaps;}
	public boolean getAutoGenerateMipMaps(){return mAutoGenerateMipMaps;}

	public void setSAddressMode(AddressMode addressMode){mSAddressMode=addressMode;}
	public AddressMode getSAddressMode(){return mSAddressMode;}

	public void setTAddressMode(AddressMode addressMode){mTAddressMode=addressMode;}
	public AddressMode getTAddressMode(){return mTAddressMode;}

	public void setRAddressMode(AddressMode addressMode){mRAddressMode=addressMode;}
	public AddressMode getRAddressMode(){return mRAddressMode;}

	public void setMinFilter(Filter minFilter){mMinFilter=minFilter;}
	public Filter getMinFilter(){return mMinFilter;}

	public void setMipFilter(Filter mipFilter){mMipFilter=mipFilter;}
	public Filter getMipFilter(){return mMipFilter;}

	public void setMagFilter(Filter magFilter){mMagFilter=magFilter;}
	public Filter getMagFilter(){return mMagFilter;}

	public void setName(String name){mName=name;}
	public String getName(){return mName;}

	// TODO: This uses android Bitmaps until we can rework the Texture system & deal with data & format
	public void setImage(android.graphics.Bitmap image){
		if(mTexturePeer!=null){
			Error.unknown(Categories.TOADLET_PEEPER,
				"Cannot change images after upload");
			return;
		}

		mDimension=Dimension.D2;
		if(image.getConfig()==android.graphics.Bitmap.Config.ALPHA_8)
			mFormat=Format.A_8;
		else if(image.getConfig()==android.graphics.Bitmap.Config.ARGB_4444)
			mFormat=Format.RGBA_4_4_4_4;
		else if(image.getConfig()==android.graphics.Bitmap.Config.ARGB_8888)
			mFormat=Format.RGBA_8;
		else if(image.getConfig()==android.graphics.Bitmap.Config.RGB_565)
			mFormat=Format.RGB_5_6_5;
		mWidth=image.getWidth();
		mHeight=image.getHeight();
		mDepth=1;
		mImage=image;
	}
	
	public android.graphics.Bitmap getImage(){return mImage;}

	public void internal_setTexturePeer(Texture.Peer texturePeer){
		if(mTexturePeer!=null){
			mTexturePeer.destroy();
		}

		mTexturePeer=texturePeer;
	}
	public Texture.Peer internal_getTexturePeer(){return mTexturePeer;}

	Type mType=Type.NORMAL;

	Dimension mDimension=Dimension.UNKNOWN;
	int mFormat=Format.UNKNOWN;
	int mWidth=0;
	int mHeight=0;
	int mDepth=0;

	boolean mAutoGenerateMipMaps=false;

	AddressMode mSAddressMode=AddressMode.REPEAT;
	AddressMode mTAddressMode=AddressMode.REPEAT;
	AddressMode mRAddressMode=AddressMode.REPEAT;

	Filter mMinFilter=Filter.NEAREST;
	Filter mMipFilter=Filter.NONE;
	Filter mMagFilter=Filter.NEAREST;

	String mName;

	android.graphics.Bitmap mImage=null;

	Texture.Peer mTexturePeer=null;
}
