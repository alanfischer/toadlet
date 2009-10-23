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
import com.motorola.iden.micro3d.*;

public class BeamEntity extends RenderableEntity{
	public BeamEntity(Engine engine,boolean highDetail){
		super(engine);
		mType|=TYPE_BEAM;

		mWorldSpace=true;

		mObject3D=new Quadrangle(new Vector3D(),new Vector3D(),new Vector3D(),new Vector3D(),null,0);
	}

	public void startBeam(int color,int scale,Vector3 start,Vector3 end){
		startBeam(color,false,null,scale,start,end);
	}

	public void startBeam(int color,boolean horizontal,String name,int scale,Vector3 start,Vector3 end){
		mScale=scale;
		mStart.set(start);
		mEnd.set(end);

		updatePoints();

		mSprite=mEngine.getSprite(name);
		if(mSprite!=null){
			mTexture=mSprite.texture;

			int x=((mSprite.startFrame)%mSprite.widthFrames)*mSprite.frameWidth;
			int y=((mSprite.startFrame)/mSprite.heightFrames)*mSprite.frameHeight;

			Quadrangle quad=(Quadrangle)mObject3D;
			if(horizontal){
				quad.setTextureCoordinates(Primitive.VERTEX_A,x,y);
				quad.setTextureCoordinates(Primitive.VERTEX_B,x,y+mSprite.frameHeight-1);
				quad.setTextureCoordinates(Primitive.VERTEX_C,x+mSprite.frameWidth-1,y+mSprite.frameHeight-1);
				quad.setTextureCoordinates(Primitive.VERTEX_D,x+mSprite.frameWidth-1,y);
			}
			else{
				quad.setTextureCoordinates(Primitive.VERTEX_A,x+mSprite.frameWidth-1,y);
				quad.setTextureCoordinates(Primitive.VERTEX_B,x,y);
				quad.setTextureCoordinates(Primitive.VERTEX_C,x,y+mSprite.frameHeight-1);
				quad.setTextureCoordinates(Primitive.VERTEX_D,x+mSprite.frameWidth-1,y+mSprite.frameHeight-1);
			}
		}
		else{
			((Primitive)mObject3D).setColor(color);
		}
	}
	
	public void setBlending(boolean blend,boolean add){
		Primitive prim=(Primitive)mObject3D;
		if(blend){
			mLayout.setSemiTransparent(true);
			if(add){
				prim.setBlendingType(Primitive.BLENDING_ADD);
			}
			else{
				prim.setBlendingType(Primitive.BLENDING_HALF);
			}
		}
		else{
			mLayout.setSemiTransparent(false);
		}
	}

	public void setTransparent(boolean t){
		((Primitive)mObject3D).enableColorKeyTransparency(t);
	}
	
	public void setStartPoint(Vector3 start){
		mStart.set(start);
		updatePoints();
	}

	public void setEndPoint(Vector3 end){
		mEnd.set(end);
		updatePoints();
	}

	void updatePoints(){
		int bits=mEngine.getSceneManager().mRenderShiftBits;
		
		Primitive prim=(Primitive)mObject3D;
		if(Math.lengthSquared(mStart,mEnd)>0){
			Math.sub(mOffset,mEnd,mStart);
			Math.cross(mOffset,Math.Z_UNIT_VECTOR3);
			Math.normalizeCarefully(mOffset,0);
			Math.mul(mOffset,mScale);

			mPoints1.set((mStart.x-mOffset.x)>>bits,(mStart.y-mOffset.y)>>bits,(mStart.z-mOffset.z)>>bits);
			mPoints2.set((mStart.x+mOffset.x)>>bits,(mStart.y+mOffset.y)>>bits,(mStart.z+mOffset.z)>>bits);
			mPoints3.set((mEnd.x+mOffset.x)>>bits,(mEnd.y+mOffset.y)>>bits,(mEnd.z+mOffset.z)>>bits);
			mPoints4.set((mEnd.x-mOffset.x)>>bits,(mEnd.y-mOffset.y)>>bits,(mEnd.z-mOffset.z)>>bits);
			
			prim.setVector(Primitive.VERTEX_A,mPoints1);
			prim.setVector(Primitive.VERTEX_B,mPoints2);
			prim.setVector(Primitive.VERTEX_C,mPoints3);
			prim.setVector(Primitive.VERTEX_D,mPoints4);
		}
	}
	
	Sprite mSprite;
	int mScale=Math.ONE;
	Vector3 mStart=new Vector3();
 	Vector3 mEnd=new Vector3();
 	Vector3 mOffset=new Vector3();
	Vector3D mPoints1=new Vector3D();
	Vector3D mPoints2=new Vector3D();
 	Vector3D mPoints3=new Vector3D();
 	Vector3D mPoints4=new Vector3D();
}
