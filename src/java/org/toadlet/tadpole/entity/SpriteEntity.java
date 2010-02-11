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

package org.toadlet.tadpole.entity;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.egg.*;
import org.toadlet.egg.Error;
import org.toadlet.peeper.*;
import org.toadlet.tadpole.*;

public class SpriteEntity extends RenderableEntity implements Renderable{
	public SpriteEntity(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		mTexture=null;
		mAnimatedTexture=null;
		mTextureMatrix.reset();

		mFrame=0;
		mWidth=0;
		mHeight=0;
		mScaled=true;

		mMaterial=null;
		mVertexData=null;
		mIndexData=null;

		mVertexBuffer=mEngine.checkoutVertexBuffer(mEngine.getVertexFormats().POSITION_TEX_COORD,4);
		mVertexData=new VertexData(mVertexBuffer);
		{
			vba.lock(mVertexBuffer,Buffer.LockType.WRITE_ONLY);

			vba.set3(0,0, -Math.HALF,Math.HALF,0);
			vba.set2(0,1, 0,0);

			vba.set3(1,0, Math.HALF,Math.HALF,0);
			vba.set2(1,1, Math.ONE,0);

			vba.set3(2,0, -Math.HALF,-Math.HALF,0);
			vba.set2(2,1, 0,Math.ONE);

			vba.set3(3,0, Math.HALF,-Math.HALF,0);
			vba.set2(3,1, Math.ONE,Math.ONE);

			vba.unlock();
		}

		mIndexData=new IndexData(IndexData.Primitive.TRISTRIP,null,0,4);

		mMaterial=new Material();
		mMaterial.setFaceCulling(Renderer.FaceCulling.NONE);
		mMaterial.setDepthWrite(false);
		
		return this;
	}

	public void destroy(){
		if(mVertexBuffer!=null){
			mEngine.checkinVertexBuffer(mVertexBuffer);
		}
		
		super.destroy();
	}
	
	public void load(scalar width,scalar height,boolean scaled,String name){
		load(width,height,scaled,mEngine.getTexture(name));
	}

	public void load(scalar width,scalar height,boolean scaled,Texture texture){
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
			mMaterial.setTextureStage(0,textureStage);
		}

		if(scaled){
			scalar hw=width/2;
			scalar hh=height/2;
			mBoundingRadius=Math.sqrt(Math.square(hw) + Math.square(hh));
		}
		else{
			mBoundingRadius=-Math.ONE;
		}

		mWidth=width;
		mHeight=height;
		mScaled=scaled;

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

	public Material getRenderMaterial(){return mMaterial;}
	public Matrix4x4 getRenderTransform(){return super.getVisualWorldTransform();}

	public void queueRenderables(Scene scene){
		Matrix4x4 scale=cache_queueRenderables_scale.reset();
		Vector4 point=cache_queueRenderables_point.reset();
		Matrix4x4 viewTransform=scene.getCamera().getViewTransform();
		Matrix4x4 projectionTransform=scene.getCamera().getProjectionTransform();

		if(mIdentityTransform){
			mVisualWorldTransform.set(mParent.getVisualWorldTransform());
		}
		else{
			Math.mul(mVisualWorldTransform,mParent.getVisualWorldTransform(),mVisualTransform);
		}

		mVisualWorldTransform.setAt(0,0,viewTransform.at(0,0));
		mVisualWorldTransform.setAt(1,0,viewTransform.at(0,1));
		mVisualWorldTransform.setAt(2,0,viewTransform.at(0,2));
		mVisualWorldTransform.setAt(0,1,viewTransform.at(1,0));
		mVisualWorldTransform.setAt(1,1,viewTransform.at(1,1));
		mVisualWorldTransform.setAt(2,1,viewTransform.at(1,2));
		mVisualWorldTransform.setAt(0,2,viewTransform.at(2,0));
		mVisualWorldTransform.setAt(1,2,viewTransform.at(2,1));
		mVisualWorldTransform.setAt(2,2,viewTransform.at(2,2));

		scale.reset();
		if(mScaled){
			scale.setAt(0,0,mWidth);
			scale.setAt(1,1,mHeight);
			scale.setAt(2,2,Math.ONE);
		}
		else{
			point.set(mVisualWorldTransform.at(0,3),mVisualWorldTransform.at(1,3),mVisualWorldTransform.at(2,3),Math.ONE);
			Math.mul(point,viewTransform);
			Math.mul(point,projectionTransform);
			scale.setAt(0,0,Math.mul(point.w,mWidth));
			scale.setAt(1,1,Math.mul(point.w,mHeight));
			scale.setAt(2,2,Math.ONE);
		}

		/// @todo  Modify this so we can have sprites of arbitrary orientations, and also have the scale passed by the
		///  transform matrix, and not having to just grab the entities scale
		scale.setAt(0,0,Math.mul(scale.at(0,0),mScale.x));
		scale.setAt(1,1,Math.mul(scale.at(1,1),mScale.y));

		Math.postMul(mVisualWorldTransform,scale);

		scene.queueRenderable(this);
	}
	
	public void render(Renderer renderer){
		renderer.renderPrimitive(mVertexData,mIndexData);
	}

	protected void updateFrame(){
		if(mAnimatedTexture!=null){
			mAnimatedTexture.getMatrix4x4ForFrame(mFrame,mTextureMatrix);
			TextureStage textureStage=new TextureStage(mAnimatedTexture.getTextureForFrame(mFrame));
			textureStage.setSAddressMode(Texture.AddressMode.CLAMP_TO_EDGE);
			textureStage.setTAddressMode(Texture.AddressMode.CLAMP_TO_EDGE);
			textureStage.setTextureMatrix(mTextureMatrix);
			mMaterial.setTextureStage(0,textureStage);
		}
	}

	public VertexBufferAccessor vba=new VertexBufferAccessor();
	
	protected Texture mTexture=null;
	protected AnimatedTexture mAnimatedTexture=null;
	protected Matrix4x4 mTextureMatrix=new Matrix4x4();

	protected int mFrame=0;
	protected scalar mWidth=0;
	protected scalar mHeight=0;
	protected boolean mScaled=false;

	protected Material mMaterial=null;
	protected VertexBuffer mVertexBuffer=null;
	protected VertexData mVertexData=null;
	protected IndexData mIndexData=null;

	protected Matrix4x4 cache_queueRenderables_scale=new Matrix4x4();
	protected Vector4 cache_queueRenderables_point=new Vector4();
}
