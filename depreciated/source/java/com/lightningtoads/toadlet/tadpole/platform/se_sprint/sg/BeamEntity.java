/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole.sg;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import com.mascotcapsule.micro3d.v3.*;

public class BeamEntity extends RenderableEntity{
	public BeamEntity(Engine engine){
		super(engine);
		mType|=TYPE_BEAM;

		mWorldSpace=true;
	}

	public void startBeam(int color,int scale,Vector3 start,Vector3 end){
		startBeam(color,false,null,scale,start,end);
	}

	public void startBeam(int color,boolean horizontal,String name,int scale,Vector3 start,Vector3 end){
		mScale=scale;
		mStart.set(start);
		mEnd.set(end);

		mColor[0]=color;

		updatePoints();

		mEffect.setTransparency(true);

		mSprite=mEngine.getSprite(name);
		if(mSprite!=null){
			mTexture=mSprite.texture;

			int x=((mSprite.startFrame)%mSprite.widthFrames)*mSprite.frameWidth;
			int y=((mSprite.startFrame)/mSprite.heightFrames)*mSprite.frameHeight;

			if(horizontal){
				mTextures[0]=x;
				mTextures[1]=y;

				mTextures[2]=x;
				mTextures[3]=y+mSprite.frameHeight-1;

				mTextures[4]=x+mSprite.frameWidth-1;
				mTextures[5]=y+mSprite.frameHeight-1;

				mTextures[6]=x+mSprite.frameWidth-1;
				mTextures[7]=y;
			}
			else{
				mTextures[0]=x+mSprite.frameWidth-1;
				mTextures[1]=y;

				mTextures[2]=x;
				mTextures[3]=y;

				mTextures[4]=x;
				mTextures[5]=y+mSprite.frameHeight-1;

				mTextures[6]=x+mSprite.frameWidth-1;
				mTextures[7]=y+mSprite.frameHeight-1;
			}
		}
		else{
			mTexture=null;
		}
	}
	
	public void setBlending(boolean blend,boolean add){
		mCommand&=~(Graphics3D.PATTR_BLEND_HALF|Graphics3D.PATTR_BLEND_ADD);
		if(blend){
			if(add){
				mCommand|=Graphics3D.PATTR_BLEND_ADD;
			}
			else{
				mCommand|=Graphics3D.PATTR_BLEND_HALF;
			}
		}
		mEffect.setSemiTransparentEnabled(blend);
	}
	
	public void setTransparent(boolean t){
		if(t){
			mCommand|=Graphics3D.PATTR_COLORKEY;
		}
		else{
			mCommand&=~Graphics3D.PATTR_COLORKEY;
		}
	}

	public void setStartPoint(Vector3 start){
		mStart.set(start);
		updatePoints();
	}

	public void setEndPoint(Vector3 end){
		mEnd.set(end);
		updatePoints();
	}

	public void render(Graphics3D g,int hwidth,int hheight){
		if(mTexture!=null){
			g.renderPrimitives(mTexture,hwidth,hheight,mLayout,mEffect,
				mCommand|Graphics3D.PDATA_TEXURE_COORD|Graphics3D.PATTR_COLORKEY,
				1,mPoints,mNormals,mTextures,mColor);
		}
		else{
			g.renderPrimitives((Texture)null,hwidth,hheight,mLayout,mEffect,
				mCommand|Graphics3D.PDATA_COLOR_PER_FACE,
				1,mPoints,mNormals,mTextures,mColor);
		}
	}

	void updatePoints(){
		int bits=mEngine.getSceneManager().mRenderShiftBits;

		if(Math.lengthSquared(mStart,mEnd)>0){
			Math.sub(mOffset,mEnd,mStart);
			Math.cross(mOffset,Math.Z_UNIT_VECTOR3);
			Math.normalizeCarefully(mOffset,0);
			Math.mul(mOffset,mScale);

			mPoints[0]=(mStart.x-mOffset.x)>>bits;
			mPoints[1]=(mStart.y-mOffset.y)>>bits;
			mPoints[2]=(mStart.z-mOffset.z)>>bits;

			mPoints[3]=(mStart.x+mOffset.x)>>bits;
			mPoints[4]=(mStart.y+mOffset.y)>>bits;
			mPoints[5]=(mStart.z+mOffset.z)>>bits;

			mPoints[6]=(mEnd.x+mOffset.x)>>bits;
			mPoints[7]=(mEnd.y+mOffset.y)>>bits;
			mPoints[8]=(mEnd.z+mOffset.z)>>bits;

			mPoints[9]=(mEnd.x-mOffset.x)>>bits;
			mPoints[10]=(mEnd.y-mOffset.y)>>bits;
			mPoints[11]=(mEnd.z-mOffset.z)>>bits;
		}
	}

	int mCommand=Graphics3D.PRIMITVE_QUADS;

	Sprite mSprite;

	int mScale=Math.ONE;
	Vector3 mStart=new Vector3();
	Vector3 mEnd=new Vector3();
	Vector3 mOffset=new Vector3();

	int[] mPoints=new int[12];
	int[] mNormals=new int[0];
	int[] mTextures=new int[8];
	int[] mColor=new int[1];
}
