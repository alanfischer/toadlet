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

package com.lightningtoads.toadlet.tadpole.widget;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.Categories;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.peeper.*;

public class SpriteWidget extends RenderableWidget{
	public SpriteWidget(Engine engine){
		super(engine);
		mType|=Type.SPRITE;

		VertexBuffer vertexBuffer=engine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,mEngine.getVertexFormats().POSITION_TEX_COORD,4));
		mVertexData=new VertexData(vertexBuffer);
		{
			VertexBufferAccessor vba=new VertexBufferAccessor();
			vba.lock(vertexBuffer,Buffer.LockType.WRITE_ONLY);

			vba.set3(	0,0, 0,Math.ONE,0);
			vba.set2(	0,1, 0,0);

			vba.set3(	1,0, Math.ONE,Math.ONE,0);
			vba.set2(	1,1, Math.ONE,0);

			vba.set3(	2,0, 0,0,0);
			vba.set2(	2,1, 0,Math.ONE);

			vba.set3(	3,0, Math.ONE,0,0);
			vba.set2(	3,1, Math.ONE,Math.ONE);

			vba.unlock();
		}

		mIndexData=new IndexData(IndexData.Primitive.TRISTRIP,null,0,4);
	}

	public void load(scalar width,scalar height,String name){
		load(width,height,mEngine.getTexture(name));
	}
	
	public void load(scalar width,scalar height,Texture texture){
		mTexture=texture;

		if(mTexture==null){
			Error.invalidParameters(Categories.TOADLET_TADPOLE,
				"Invalid Texture");
			return;
		}

		if(mTexture.getType()==Texture.Type.ANIMATED){
			mAnimatedTexture=(AnimatedTexture)texture;
		}
		else{
			mAnimatedTexture=null;
			mTextureMatrix.set(Math.IDENTITY_MATRIX4X4);
			TextureStage textureStage=new TextureStage(mTexture);
			textureStage.setSAddressMode(Texture.AddressMode.CLAMP_TO_EDGE);
			textureStage.setTAddressMode(Texture.AddressMode.CLAMP_TO_EDGE);
			textureStage.setTextureMatrix(mTextureMatrix);
			mTextureStage=textureStage;
		}

		setFractionalSize(width,width,false,true);

		updateFrame();
	}

	public void setFrame(int frame){
		mFrame=frame;
		updateFrame();
	}

	public int getNumFrames(){
		int numFrames=1;
		if(mAnimatedTexture!=null){
			numFrames=mAnimatedTexture.getNumFrames();
		}
		return numFrames;
	}

	public void render(Renderer renderer){
		renderer.setTextureStage(0,mTextureStage);

		renderer.renderPrimitive(mVertexData,mIndexData);
	}
	
	public boolean applyScale(){return true;}

	public TextureStage getTextureStage(){return mTextureStage;}

	protected void updateFrame(){
		if(mAnimatedTexture!=null){
			mAnimatedTexture.getMatrix4x4ForFrame(mFrame,mTextureMatrix);
			TextureStage textureStage=new TextureStage(mAnimatedTexture.getTextureForFrame(mFrame));
			textureStage.setSAddressMode(Texture.AddressMode.CLAMP_TO_EDGE);
			textureStage.setTAddressMode(Texture.AddressMode.CLAMP_TO_EDGE);
			textureStage.setTextureMatrix(mTextureMatrix);
			mTextureStage=textureStage;
		}
	}

	protected Texture mTexture=null;
	protected AnimatedTexture mAnimatedTexture=null;
	protected Matrix4x4 mTextureMatrix=new Matrix4x4();

	protected int mFrame=0;

	protected VertexData mVertexData=null;
	protected IndexData mIndexData=null;
	protected TextureStage mTextureStage=null;
}
