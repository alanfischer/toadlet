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
import javax.microedition.khronos.opengles.*;
import java.util.*;

public class ParticleEntity extends RenderableEntity{
	public int interpolateColor(int startColor,int endColor,int t){
		int a=((startColor&0xFF000000)>>24)&0xFF;
		int r=((startColor&0x00FF0000)>>16)&0xFF;
		int g=((startColor&0x0000FF00)>>8)&0xFF;
		int b=(startColor&0x000000FF);

		a=(Math.fixedToInt(Math.mul(Math.intToFixed(a),t))&0xFF);
		r=(Math.fixedToInt(Math.mul(Math.intToFixed(r),t))&0xFF);
		g=(Math.fixedToInt(Math.mul(Math.intToFixed(g),t))&0xFF);
		b=(Math.fixedToInt(Math.mul(Math.intToFixed(b),t))&0xFF);

		return (a<<24) | (r<<16) | (g<<8) | b;
	}

	protected class Particle{
		public Particle(){}
	
		public boolean visible;
		public Vector3 position=new Vector3();
		public Vector3 velocity=new Vector3();
		public int dieTime;
		public int age; // From 0 to Math.ONE
	};

	public ParticleEntity(Engine engine){
		super(engine);
		mType|=TYPE_PARTICLE;

		mMaterial=new Material();
		mMaterial.setFaceCulling(Material.FC_NONE);

		mSceneManager=mEngine.getSceneManager();
		mSceneManager.registerParticleEntity(this);
	}

	public void destroy(){
		if(mSceneManager!=null){
			mSceneManager.unregisterParticleEntity(this);
		}

		super.destroy();
	}

	public void startParticles(int startColor,int endColor,int startScale,int endScale,int count,int life,Vector3 velocity,Vector3 variance,boolean burst,int initialDelay){
		int i;

		mStartColor=startColor;
		mEndColor=endColor;
		mStartScale=startScale;
		mEndScale=endScale;
		mLife=life;
		mVelocity.set(velocity);
		mVariance.set(variance);
		mBurst=burst;

		mParticles=new Particle[count];
		int time=mSceneManager.getTime();
		if(burst){
			for(i=mParticles.length-1;i>=0;--i){
				mParticles[i]=new Particle();
				mParticles[i].visible=false;
				mParticles[i].dieTime=time+initialDelay;
				mParticles[i].age=0;
			}
		}
		else{
			for(i=mParticles.length-1;i>=0;--i){
				mParticles[i]=new Particle();
				mParticles[i].visible=false;
				mParticles[i].dieTime=time+life*i/count;
				mParticles[i].age=0;
			}
		}
	
		VertexFormat vertexFormat=new VertexFormat(2);
		int formatBit=VertexElement.FORMAT_BIT_FIXED_32;
		vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_POSITION,formatBit|VertexElement.FORMAT_BIT_COUNT_3));
		vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_COLOR,VertexElement.FORMAT_COLOR_RGBA));
		Buffer vertexBuffer=new Buffer(Buffer.UT_DYNAMIC,Buffer.AT_WRITE_ONLY,vertexFormat,count*4);
		mVertexData=new VertexData(vertexBuffer);

		Buffer indexBuffer=new Buffer(Buffer.UT_STATIC,Buffer.AT_WRITE_ONLY,Buffer.IDT_UINT_16,count*6);
		mIndexData=new IndexData(IndexData.PRIMITIVE_TRIS,indexBuffer,0,count*6);

		{
			java.nio.ShortBuffer data=(java.nio.ShortBuffer)indexBuffer.lock(Buffer.LT_WRITE_ONLY);

			for(i=0;i<count;++i){
				data.put(i*6+0,(short)(i*4+0));
				data.put(i*6+1,(short)(i*4+1));
				data.put(i*6+2,(short)(i*4+2));
				data.put(i*6+3,(short)(i*4+2));
				data.put(i*6+4,(short)(i*4+3));
				data.put(i*6+5,(short)(i*4+0));
			}

			indexBuffer.unlock();
		}
	}

	public void setGravity(Vector3 gravity){
		mGravity.set(gravity);
	}

	public void setDieTime(int dieTime){
		mDieTime=dieTime;
	}

	public void setPaused(boolean paused){
		mPaused=paused;
	}

	public void setBlending(boolean blend,boolean alpha){
		if(blend){
			if(alpha){
				mMaterial.setBlend(Material.COMBINATION_ALPHA);
			}
			else{
				mMaterial.setBlend(Material.COMBINATION_COLOR);
			}
			mMaterial.setDepthWrite(false);
		}
		else{
			mMaterial.setBlend(Material.DISABLED_BLEND);
			mMaterial.setDepthWrite(true);
		}
	}

	public Material getMaterial(){return mMaterial;}

	void updateAnimation(int dt){
		int i;

		int time=mSceneManager.getTime();
		boolean alive=((mDieTime==0) || time<mDieTime);
		boolean allDead=!alive;
		alive=alive && (!mPaused);

		int fdt=Math.milliToFixed(dt);

		updateWorldTransform(this);
		Vector3 translate=mWorldTranslate;
		Matrix3x3 rotate=mWorldRotate;
		Vector3 velocity=mVelocity;

		int baseTime=((time/mLife)+1)*mLife;
		for(i=mParticles.length-1;i>=0;--i){
			Particle p=mParticles[i];
			if(p.dieTime<=time){
				if(alive){
					p.visible=true;
					if(p.dieTime==0){
						p.dieTime=time+mLife;
						p.age=0;
					}
					else{
						p.dieTime=baseTime+p.dieTime%mLife;
						p.age=Math.ONE-Math.div(Math.milliToFixed(p.dieTime-time),Math.milliToFixed(mLife));
					}
					p.position.set(translate);
					p.velocity.set(velocity);
					p.velocity.x+=mRandom.nextInt(mVariance.x*2)-mVariance.x;
					p.velocity.y+=mRandom.nextInt(mVariance.y*2)-mVariance.y;
					p.velocity.z+=mRandom.nextInt(mVariance.z*2)-mVariance.z;
					Math.mul(p.velocity,rotate);
				}
				else{
					p.visible=false;
				}
			}
			else{
				allDead=false;

				p.age=Math.ONE-Math.div(Math.milliToFixed(p.dieTime-time),Math.milliToFixed(mLife));
				Math.mul(mCacheVector3,mGravity,fdt);
				Math.add(p.velocity,mCacheVector3);
				Math.mul(mCacheVector3,p.velocity,fdt);
				Math.add(p.position,mCacheVector3);
			}
		}

		if(allDead){
			destroy();
			return;
		}
	}

	void updateVertexBuffer(CameraEntity camera){
		int i;
		Matrix4x4 viewTransform=camera.getViewTransform();

		int rightX=viewTransform.at(0,0);
		int rightY=viewTransform.at(0,1);
		int rightZ=viewTransform.at(0,2);

		int upX=viewTransform.at(1,0);
		int upY=viewTransform.at(1,1);
		int upZ=viewTransform.at(1,2);

		{
			java.nio.IntBuffer data=(java.nio.IntBuffer)mVertexData.getVertexBuffer(0).lock(Buffer.LT_WRITE_ONLY);

			for(i=mParticles.length-1;i>=0;--i){
				Particle p=mParticles[i];
				Vector3 pos=p.position;
				int color=interpolateColor(mStartColor,mEndColor,p.age);
				color=((color&0x00FFFFFF)<<8) | (((color&0xFF000000)>>24)&0xFF); // Convert to RGBA for GLES
				int scale=Math.mul(mEndScale-mStartScale,p.age)+mStartScale;

				int vi=i*16;
				if(p.visible){
					data.put(vi+0,pos.x+Math.mul(-upX-rightX,scale));
					data.put(vi+1,pos.y+Math.mul(-upY-rightY,scale));
					data.put(vi+2,pos.z+Math.mul(-upZ-rightZ,scale));

					data.put(vi+4,pos.x+Math.mul(upX-rightX,scale));
					data.put(vi+5,pos.y+Math.mul(upY-rightY,scale));
					data.put(vi+6,pos.z+Math.mul(upZ-rightZ,scale));

					data.put(vi+8,pos.x+Math.mul(upX+rightX,scale));
					data.put(vi+9,pos.y+Math.mul(upY+rightY,scale));
					data.put(vi+10,pos.z+Math.mul(upZ+rightZ,scale));

					data.put(vi+12,pos.x+Math.mul(-upX+rightX,scale));
					data.put(vi+13,pos.y+Math.mul(-upY+rightY,scale));
					data.put(vi+14,pos.z+Math.mul(-upZ+rightZ,scale));

					data.put(vi+3,color);
					data.put(vi+7,color);
					data.put(vi+11,color);
					data.put(vi+15,color);
				}
				else{
					data.put(vi+0,0);
					data.put(vi+1,0);
					data.put(vi+2,0);

					data.put(vi+4,0);
					data.put(vi+5,0);
					data.put(vi+6,0);

					data.put(vi+8,0);
					data.put(vi+9,0);
					data.put(vi+10,0);

					data.put(vi+12,0);
					data.put(vi+13,0);
					data.put(vi+14,0);
				}
			}

			mVertexData.getVertexBuffer(0).unlock();
		}
	}

	void updateWorldTransform(Entity entity){
		if(entity.getParent()==null){
			mWorldTranslate.set(entity.getTranslate());
			mWorldRotate.set(entity.getRotate());
		}
		else{
			updateWorldTransform(entity.getParent());

			if(entity.isIdentityTransform()==false){
				Math.mul(mCacheVector3,mWorldRotate,entity.getTranslate());
				Math.add(mWorldTranslate,mCacheVector3);

				Math.mul(mCacheMatrix3x3,mWorldRotate,entity.getRotate());
				mWorldRotate.set(mCacheMatrix3x3);
			}
		}
	}

	int mStartColor=0xFFFFFFFF;
	int mEndColor=0xFFFFFFFF;
	int mStartScale=Math.ONE;
	int mEndScale=Math.ONE;
	boolean mBurst=false;
	int mLife=0;
	Vector3 mVelocity=new Vector3();
	Vector3 mVariance=new Vector3();

	Vector3 mGravity=new Vector3();

	boolean mPaused=false;
	int mDieTime=0;

	Particle[] mParticles;

	Material mMaterial;
	VertexData mVertexData;
	IndexData mIndexData;
	Random mRandom=new Random();

	Vector3 mWorldTranslate=new Vector3();
	Matrix3x3 mWorldRotate=new Matrix3x3();

	Vector3 mCacheVector3=new Vector3();
	Matrix3x3 mCacheMatrix3x3=new Matrix3x3();
	Vector3 mCacheVelocity=new Vector3();

	SceneManager mSceneManager;
}
