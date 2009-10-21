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

package com.lightningtoads.toadlet.tadpole.entity;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;
import java.util.*;

#if defined(TOADLET_FIXED_POINT)
	#include <com/lightningtoads/toadlet/egg/mathfixed/Inline.h>

	import static com.lightningtoads.toadlet.egg.mathfixed.Math.*;

	#define mul_ss(s1,s2)	TOADLET_MUL_XX(s1,s2)
	#define mul_vvs(r,v,s)	r.x=TOADLET_MUL_XX(v.x,s);r.y=TOADLET_MUL_XX(v.y,s);r.z=TOADLET_MUL_XX(v.z,s)
	#define mul_vs(r,s)		r.x=TOADLET_MUL_XX(r.x,s);r.y=TOADLET_MUL_XX(r.y,s);r.z=TOADLET_MUL_XX(r.z,s)

	#define div_ss(s1,s2)	TOADLET_DIV_XX(s1,s2)
	
	#define cross_vvv(r,v1,v2) \
		r.x=TOADLET_MUL_XX(v1.y,v2.z) - TOADLET_MUL_XX(v1.z,v2.y); \
		r.y=TOADLET_MUL_XX(v1.z,v2.x) - TOADLET_MUL_XX(v1.x,v2.z); \
		r.z=TOADLET_MUL_XX(v1.x,v2.y) - TOADLET_MUL_XX(v1.y,v2.x);

	#define lerp_sss(t1,t2,time) (TOADLET_MUL_XX(t1,ONE-time) + TOADLET_MUL_XX(t2,time));
	#define lerp_ccs(start,end,time) \
			((int)((TOADLET_MUL_XX((start&0xFF000000)>>>24,ONE-time) + TOADLET_MUL_XX((end&0xFF000000)>>>24,time))<<24)) | \
			((int)((TOADLET_MUL_XX((start&0x00FF0000)>>>16,ONE-time) + TOADLET_MUL_XX((end&0x00FF0000)>>>16,time))<<16)) | \
			((int)((TOADLET_MUL_XX((start&0x0000FF00)>>>8 ,ONE-time) + TOADLET_MUL_XX((end&0x0000FF00)>>>8 ,time))<<8 )) | \
			((int)((TOADLET_MUL_XX((start&0x000000FF)     ,ONE-time) + TOADLET_MUL_XX((end&0x000000FF)     ,time))    ))
#else
	import static com.lightningtoads.toadlet.egg.math.Math.*;

	#define mul_ss(s1,s2)	(s1*s2)
	#define mul_vvs(r,v,s)	r.x=(v.x*s);r.y=(v.y*s);r.z=(v.z*s)
	#define mul_vs(r,s)		r.x=(r.x*s);r.y=(r.y*s);r.z=(r.z*s)

	#define div_ss(s1,s2)	(s1/s2)

	#define cross_vvv(r,v1,v2) \
		r.x=v1.y*v2.z-v1.z*v2.y; \
		r.y=v1.z*v2.x-v1.x*v2.z; \
		r.z=v1.x*v2.y-v1.y*v2.x;

	#define lerp_sss(t1,t2,time) (t1*(ONE-time) + t2*time);
	#define lerp_ccs(start,end,time) \
			((int)(((((start&0xFF000000)>>>24)*ONE-time) + (((end&0xFF000000)>>>24)*time))<<24)) | \
			((int)(((((start&0x00FF0000)>>>16)*ONE-time) + (((end&0x00FF0000)>>>16)*time))<<16)) | \
			((int)(((((start&0x0000FF00)>>>8 )*ONE-time) + (((end&0x0000FF00)>>>8 )*time))<<8 )) | \
			((int)(((((start&0x000000FF)     )*ONE-time) + (((end&0x000000FF)     )*time))    ))
#endif

public class ParticleEntity extends RenderableEntity implements Renderable{
	public class Particle{
		public scalar x,y,z; // local space
		public scalar ox,oy,oz; // orientation vector
		public scalar vx,vy,vz; // velocity vector
		public byte frame;
		public int startColor,endColor;
		public scalar startScale,endScale;
		public scalar age;
		public boolean visible;
	};

	public interface ParticleSimulator{
		public void setParticleLife(int life);
		public void setParticleInitialVelocity(Vector3 velocity,Vector3 variance);
		public void setSpawnAABox(AABox box);
		public void setCoefficientOfGravity(scalar cog);

		public void updateParticles(int dt,Matrix4x4 transform);
	}

	public ParticleEntity(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		mDestroyNextLogicFrame=false;

		mWorldSpace=false;
		mOrientation=false;
		mIndividualAttributes=false;
		mParticlesPerBeam=0;
		mDestroyOnFinish=false;
		mHadPoints=false;
		mHadAges=false;
		mHasColor=false;
		mHasScale=false;
		mLineBeams=false;

		mRandomStartColor=false;
		mStartColor.reset();
		mEndColor.reset();
		mStartScale=0;
		mEndScale=0;

		mTextureSection=null;
		mMaterial=null;
		mVertexBuffer=null;
		mVertexData=null;
		mIndexBuffer=null;
		mIndexData=null;
		mLineIndexData=null;

		mWorldTransform.reset();

		mUpdateParticles=true;
		mParticleSimulator=null;

		mHasIdealViewTransform=false;
		mIdealViewTransform.reset();

		mEpsilon=
			#if defined(TOADLET_FIXED_POINT)
				512;
			#else
				0.01f;
			#endif
	
		mRandom.setSeed(System.currentTimeMillis());
		
		return this;
	}
	
	public void destroy(){
		stopSimulating();

		destroyBuffers();
		
		super.destroy();
	}

	public void setScale(scalar x,scalar y,scalar z){super.setScale(x,y,z);}
	public void setScale(Vector3 scale){super.setScale(scale);}

	public boolean start(int particlesPerBeam,int numParticles,boolean hasColor,List<Vector3> points,List<Scalar> ages){return start(particlesPerBeam,numParticles,hasColor,points,ages,0,true);}
	public boolean start(int particlesPerBeam,int numParticles,boolean hasColor,List<Vector3> points,List<Scalar> ages,scalar defaultAge){return start(particlesPerBeam,numParticles,hasColor,points,ages,defaultAge,true);}
	public boolean start(int particlesPerBeam,int numParticles,boolean hasColor,List<Vector3> points,List<Scalar> ages,scalar defaultAge,boolean visible){
		if(particlesPerBeam>1 && (numParticles%particlesPerBeam)!=0){
			Error.invalidParameters(Categories.TOADLET_TADPOLE,
				"startParticles: Must specify a number of particles divisible by particlesPerBeam");
			return false;
		}

		mParticlesPerBeam=particlesPerBeam;

		int numFrames=1;
		if(mTextureSection!=null){
			numFrames=mTextureSection.getNumFrames();
		}

		int startColor=mStartColor.getABGR();
		int endColor=mEndColor.getABGR();
		scalar startScale=mStartScale/2;
		scalar endScale=mEndScale/2;

		mNumParticles=numParticles;
		if(mParticles.length<mNumParticles){
			Particle[] particles=new Particle[mNumParticles];
			System.arraycopy(mParticles,0,particles,0,mParticles.length);
			int i;
			for(i=mParticles.length;i<mNumParticles;++i){
				particles[i]=new Particle();
			}
			mParticles=particles;
		}

		int i;
		if(points!=null){
			mBoundingRadius=0;
			if(ages!=null){
				for(i=0;i<numParticles;++i){
					Particle p=mParticles[i];
					Vector3 point=points.get(i);

					p.x=point.x;
					p.y=point.y;
					p.z=point.z;
					p.frame=(byte)(i%numFrames);
					p.startColor=mRandomStartColor?randomColor():startColor;
					p.endColor=endColor;
					p.startScale=startScale;
					p.endScale=endScale;
					p.age=ages.get(i);
					p.visible=visible;

					scalar ls=lengthSquared(point);
					if(mBoundingRadius<ls) mBoundingRadius=ls;
				}
			}
			else{
				for(i=0;i<numParticles;++i){
					Particle p=mParticles[i];
					Vector3 point=points.get(i);

					p.x=point.x;
					p.y=point.y;
					p.z=point.z;
					p.frame=(byte)(i%numFrames);
					p.startColor=mRandomStartColor?randomColor():startColor;
					p.endColor=endColor;
					p.startScale=startScale;
					p.endScale=endScale;
					p.age=defaultAge>=0?defaultAge:div(fromInt(i),fromInt(numParticles));
					p.visible=visible;

					scalar ls=lengthSquared(point);
					if(mBoundingRadius<ls) mBoundingRadius=ls;
				}
			}
			mBoundingRadius=sqrt(mBoundingRadius);
		}
		else if(ages!=null){
			for(i=0;i<numParticles;++i){
				Particle p=mParticles[i];

				p.x=0;
				p.y=0;
				p.z=0;
				p.frame=(byte)(i%numFrames);
				p.startColor=mRandomStartColor?randomColor():startColor;
				p.endColor=endColor;
				p.startScale=startScale;
				p.endScale=endScale;
				p.age=ages.get(i);
				p.visible=visible;
			}
		}
		else{
			for(i=0;i<numParticles;++i){
				Particle p=mParticles[i];
				p.x=0;
				p.y=0;
				p.z=0;
				p.frame=(byte)(i%numFrames);
				p.startColor=mRandomStartColor?randomColor():startColor;
				p.endColor=endColor;
				p.startScale=startScale;
				p.endScale=endScale;
				p.age=defaultAge>=0?defaultAge:div(fromInt(i),fromInt(numParticles));
				p.visible=visible;
			}
		}

		mHadPoints=(points!=null);
		mHadAges=(ages!=null) || defaultAge!=0;
		mHasColor=hasColor;
		mHasScale=false;

		createBuffers();

		mUpdateParticles=true;

		return true;
	}
	
	public void setRandomStartColor(boolean random){mRandomStartColor=random;}
	public void setStartColor(Color startColor){mStartColor.set(startColor);mUpdateParticles=true;}
	public Color getStartColor(){return mStartColor;}
	public void setEndColor(Color endColor){mEndColor.set(endColor);mUpdateParticles=true;}
	public Color getEndColor(){return mEndColor;}
	public void setColor(Color startColor,Color endColor){mStartColor.set(startColor);mEndColor.set(endColor);mUpdateParticles=true;}
	public void setColor(Color color){mStartColor.set(color);mEndColor.set(color);mUpdateParticles=true;}

	public void setStartScale(scalar startScale){mStartScale=startScale;mUpdateParticles=true;}
	public scalar getStartScale(){return mStartScale;}
	public void setEndScale(scalar endScale){mEndScale=endScale;mUpdateParticles=true;}
	public scalar getEndScale(){return mEndScale;}
	public void setScale(scalar startScale,scalar endScale){mStartScale=startScale;mEndScale=endScale;mUpdateParticles=true;}
	public void setScale(scalar scale){mStartScale=scale;mEndScale=scale;mUpdateParticles=true;}

	public void setWorldSpace(boolean worldSpace){
		mWorldSpace=worldSpace;

		updateWorldTransform(this);
		Matrix4x4 m=mWorldTransform;
		int i;
		for(i=0;i<mNumParticles;++i){
			Particle p=mParticles[i];
			scalar x=mul_ss(m.at(0,0),p.x) + mul_ss(m.at(0,1),p.y) + mul_ss(m.at(0,2),p.z) + m.at(0,3);
			scalar y=mul_ss(m.at(1,0),p.x) + mul_ss(m.at(1,1),p.y) + mul_ss(m.at(1,2),p.z) + m.at(1,3);
			scalar z=mul_ss(m.at(2,0),p.x) + mul_ss(m.at(2,1),p.y) + mul_ss(m.at(2,2),p.z) + m.at(2,3);
			p.x=x;
			p.y=y;
			p.z=z;
		}
	}
	
	public boolean getWorldSpace(){return mWorldSpace;}

	public void setOrientation(boolean vector){mOrientation=vector;}
	public boolean getOrientation(){return mOrientation;}

	public void setTextureSection(TextureSection textureSection){mTextureSection=textureSection;}
	public TextureSection getTextureSection(){return mTextureSection;}

	public void setIndividualAttributes(boolean individual){mIndividualAttributes=individual;}
	public boolean getIndividualAttributes(){return mIndividualAttributes;}

	public int getNumParticles(){return mNumParticles;}
	public Particle getParticle(int i){return mParticles[i];}

	public boolean hadPoints(){return mHadPoints;}
	public boolean hadAges(){return mHadAges;}

	public void setRenderMaterial(Material material){mMaterial=material;}

	public void setDestroyOnFinish(boolean destroy){mDestroyOnFinish=destroy;}
	public boolean getDestroyOnFinish(){return mDestroyOnFinish;}

	public void setIdealViewTransform(Matrix4x4 transform){mIdealViewTransform.set(transform);mHasIdealViewTransform=true;}

	public void setLineBeams(boolean lineBeams){mLineBeams=lineBeams;mUpdateParticles=true;}
	
	public void startSimulating(){startSimulating(null);}
	public void startSimulating(ParticleSimulator particleSimulator){
		if(particleSimulator==null){
			particleSimulator=mEngine.getScene().newParticleSimulator(this);
		}
		mParticleSimulator=particleSimulator;

		// Disable the bounding radius, cause they may very well bounce out of it
		mBoundingRadius=-ONE;

		if(mVertexBuffer!=null){
			mVertexBuffer.setRememberContents(false);
		}
		setReceiveUpdates(true);
	}

	public void stopSimulating(){
		if(mVertexBuffer!=null){
			mVertexBuffer.setRememberContents(true);
		}
		setReceiveUpdates(false);

		mParticleSimulator=null;
	}
	
	public ParticleSimulator getParticleSimulator(){return mParticleSimulator;}
	public void destroyNextLogicFrame(){mDestroyNextLogicFrame=true;}
	
	public void logicUpdate(int dt){
		super.logicUpdate(dt);

		if(mDestroyNextLogicFrame){
			destroy();
		}
	}

	public void visualUpdate(int dt){
		if(mParticleSimulator!=null){
			if(mWorldSpace){
				mul(mVisualWorldTransform,mParent.getVisualWorldTransform(),mVisualTransform);
				mParticleSimulator.updateParticles(dt,mVisualWorldTransform);
			}
			else{
				mParticleSimulator.updateParticles(dt,mVisualTransform);
			}
			mUpdateParticles=true;
		}
	}

	public void queueRenderables(Scene scene){
		if(mUpdateParticles){
			if(mHasIdealViewTransform){
				updateVertexBuffer(mIdealViewTransform);
			}
			else{
				updateVertexBuffer(scene.getIdealParticleViewTransform());
			}
			mUpdateParticles=false;
		}

		scene.queueRenderable(this);
	}

	public Material getRenderMaterial(){return mMaterial;}

	public Matrix4x4 getRenderTransform(){
		if(mWorldSpace){
			return IDENTITY_MATRIX4X4;
		}
		else{
			return super.getVisualWorldTransform();
		}
	}

	public void render(Renderer renderer){
		if(mParticlesPerBeam==1) renderer.setPointParameters(true,Math.fromInt(50),false,0,0,0,0,0);
		renderer.renderPrimitive(mVertexData,mLineBeams?mLineIndexData:mIndexData);
		if(mParticlesPerBeam==1) renderer.setPointParameters(false,Math.fromInt(50),false,0,0,0,0,0);
	}

	public VertexBufferAccessor vba=new VertexBufferAccessor();
	public IndexBufferAccessor iba=new IndexBufferAccessor();

	protected int randomColor(){return 0xFF000000 | (mRandom.nextInt(0xFF)<<16) | (mRandom.nextInt(0xFF)<<8) | mRandom.nextInt(0xFF);}

	protected void updateWorldTransform(Entity entity){
		if(entity.getParent()==null){
			setMatrix4x4FromTranslateRotateScale(mWorldTransform,entity.getTranslate(),entity.getRotate(),entity.getScale());
		}
		else{
			updateWorldTransform(entity.getParent());

			if(entity.isIdentityTransform()==false){
				Matrix4x4 temp1=cache_updateWorldTransform_temp1;
				Matrix4x4 temp2=cache_updateWorldTransform_temp2;

				setMatrix4x4FromTranslateRotateScale(temp1,entity.getTranslate(),entity.getRotate(),entity.getScale());
				mul(temp2,mWorldTransform,temp1);
				mWorldTransform.set(temp2);
			}
		}
	}

	protected final void createBuffers(){
		int i,j,ii,vi;

		destroyBuffers();
		
		int numParticles=mNumParticles;

		VertexFormat vertexFormat;
		if(mParticlesPerBeam==1){
			if(mHasColor==false){
				vertexFormat=mEngine.getVertexFormats().POSITION;
			}
			else{
				vertexFormat=mEngine.getVertexFormats().POSITION_COLOR;
			}
		}
		else{
			if(mHasColor==false){
				vertexFormat=mEngine.getVertexFormats().POSITION_TEX_COORD;
			}
			else{
				vertexFormat=mEngine.getVertexFormats().POSITION_COLOR_TEX_COORD;
			}
		}

		int numVertexes=0;
		int numIndexes=0;
		if(mParticlesPerBeam==0){
			numVertexes=numParticles*4;
			numIndexes=numParticles*6;
		}
		else if(mParticlesPerBeam==1){
			numVertexes=numParticles;
			numIndexes=numParticles;
		}
		else{
			// Use enough vertexes for 4 on each beam end and 2 in between
			numVertexes=(numParticles/mParticlesPerBeam)*((mParticlesPerBeam-2)*2+8);
			// Use enough indexes for a 6 on each beam end and 6 in between
			numIndexes=(numParticles/mParticlesPerBeam)*((mParticlesPerBeam-2)*2+6)*3;
		}

		mVertexBuffer=mEngine.checkoutVertexBuffer(vertexFormat,numVertexes);
		mVertexBuffer.setRememberContents(true);
		mVertexData=new VertexData(mVertexBuffer);

		if(mParticlesPerBeam==1){
			mIndexData=new IndexData(IndexData.Primitive.POINTS,null,0,numIndexes);
		}
		else{
			mIndexBuffer=mEngine.checkoutIndexBuffer(numVertexes<256?IndexBuffer.IndexFormat.UINT_8:IndexBuffer.IndexFormat.UINT_16,numIndexes);
			mIndexBuffer.setRememberContents(true);
			mIndexData=new IndexData(IndexData.Primitive.TRIS,mIndexBuffer,0,numIndexes);
			mLineIndexData=new IndexData(IndexData.Primitive.LINES,null,0,numParticles);

			iba.lock(mIndexBuffer,Buffer.LockType.WRITE_ONLY);

			if(mParticlesPerBeam<2){
				for(i=0;i<numParticles;++i){
					ii=i*6;

					iba.set(ii+0,i*4+0);
					iba.set(ii+1,i*4+1);
					iba.set(ii+2,i*4+2);
					iba.set(ii+3,i*4+2);
					iba.set(ii+4,i*4+3);
					iba.set(ii+5,i*4+1);
				}
			}
			else{
				int indexesPerBeam=((mParticlesPerBeam-2)*2+6)*3;
				int vertexesPerBeam=((mParticlesPerBeam-2)*2+8);
				for(i=0;i<numParticles/mParticlesPerBeam;i++){
					ii=i*indexesPerBeam;
					vi=i*vertexesPerBeam;

					iba.set(ii+0,vi+0);
					iba.set(ii+1,vi+1);
					iba.set(ii+2,vi+2);
					iba.set(ii+3,vi+2);
					iba.set(ii+4,vi+3);
					iba.set(ii+5,vi+1);

					for(j=0;j<mParticlesPerBeam-1;++j){
						iba.set(ii+j*6+6 ,vi+j*2+2);
						iba.set(ii+j*6+7 ,vi+j*2+3);
						iba.set(ii+j*6+8 ,vi+j*2+4);
						iba.set(ii+j*6+9 ,vi+j*2+4);
						iba.set(ii+j*6+10,vi+j*2+5);
						iba.set(ii+j*6+11,vi+j*2+3);
						
					}

					iba.set(ii+indexesPerBeam-6,vi+vertexesPerBeam-4);
					iba.set(ii+indexesPerBeam-5,vi+vertexesPerBeam-3);
					iba.set(ii+indexesPerBeam-4,vi+vertexesPerBeam-2);
					iba.set(ii+indexesPerBeam-3,vi+vertexesPerBeam-2);
					iba.set(ii+indexesPerBeam-2,vi+vertexesPerBeam-1);
					iba.set(ii+indexesPerBeam-1,vi+vertexesPerBeam-3);
				}
			}

			iba.unlock();
		}
	}
	
	protected final void destroyBuffers(){
		if(mVertexBuffer!=null){
			mEngine.checkinVertexBuffer(mVertexBuffer);
			mVertexBuffer=null;
		}
		
		if(mIndexBuffer!=null){
			mEngine.checkinIndexBuffer(mIndexBuffer);
			mIndexBuffer=null;
		}
	}
	
	protected final void updateVertexBuffer(Matrix4x4 viewTransform){
		int i,j;

		Vector3 viewUp=cache_updateVertexBuffer_viewUp,viewRight=cache_updateVertexBuffer_viewRight,viewForward=cache_updateVertexBuffer_viewForward;
//		setVectorsFromMatrix4x4(viewTransform,viewRight,viewUp,viewForward);
setAxesFromMatrix4x4(viewTransform,viewRight,viewUp,viewForward);

		Vector3 up=cache_updateVertexBuffer_up,right=cache_updateVertexBuffer_right,forward=cache_updateVertexBuffer_forward;
		int startColor=mStartColor.getABGR();
		int endColor=mEndColor.getABGR();
		int color=startColor;
		scalar startScale=mStartScale/2;
		scalar endScale=mEndScale/2;
		scalar scale=startScale;
		{
			vba.lock(mVertexData.getVertexBuffer(0),Buffer.LockType.WRITE_ONLY);
			java.nio.ByteBuffer buffer=vba.getData();

			if(mParticlesPerBeam==1){
				for(i=mNumParticles-1;i>=0;--i){
					Particle p=mParticles[i];

					if(p.visible==false){
						color=0x00000000;
						if(mHasScale) scale=0;
					}
					else if(mIndividualAttributes){
						color=lerp_ccs(p.startColor,p.endColor,p.age);
						if(mHasScale) scale=lerp_sss(p.startScale,p.endScale,p.age);
					}
					else{
						color=lerp_ccs(startColor,endColor,p.age);
						if(mHasScale) scale=lerp_sss(startScale,endScale,p.age);
					}

					if(mHasColor){
						buffer.putInt(p.x);buffer.putInt(p.y);buffer.putInt(p.z);
						//vba.set3(vi+0,0,p.x,p.y,p.z);
						buffer.putInt(color);
						//vba.setABGR(vi+0,1,color);
					}
					else{
						buffer.putInt(p.x);buffer.putInt(p.y);buffer.putInt(p.z);
						//vba.set3(vi+0,0,p.x,p.y,p.z);
					}
				}
			}
			else if(mParticlesPerBeam==0){
				int frameOffset=0,widthFrames=1,heightFrames=1;
				if(mTextureSection!=null){
					frameOffset=mTextureSection.getFrameOffset();
					widthFrames=mTextureSection.getWidthFrames();
					heightFrames=mTextureSection.getHeightFrames();
				}

				for(i=mNumParticles-1;i>=0;--i){
					Particle p=mParticles[i];

					if(p.visible==false){
						color=0x00000000;
						if(mHasScale) scale=0;
					}
					else if(mIndividualAttributes){
						color=lerp_ccs(p.startColor,p.endColor,p.age);
						if(mHasScale) scale=lerp_sss(p.startScale,p.endScale,p.age);
					}
					else{
						color=lerp_ccs(startColor,endColor,p.age);
						if(mHasScale) scale=lerp_sss(startScale,endScale,p.age);
					}

					if(mOrientation){
						viewUp.set(p.ox,p.oy,p.oz);
						if(mWorldSpace==false){
							Math.mul(viewUp,mVisualWorldTransform);
						}
						mul_vvs(up,viewUp,scale);
						mul_vvs(right,viewRight,scale);
					}
					else{
						mul_vvs(up,viewUp,scale);
						mul_vvs(right,viewRight,scale);
					}

					scalar tx0,ty0,tx1,ty1;

					int v=(p.frame+frameOffset)%widthFrames;
					scalar s=div_ss(ONE,fromInt(widthFrames));
					tx0=mul_ss(fromInt(v),s);
					tx1=tx0+s;

					v=((p.frame+frameOffset)/widthFrames)%heightFrames;
					s=div_ss(ONE,fromInt(heightFrames));
					ty0=mul_ss(fromInt(v),s);
					ty1=ty0+s;

					int vi=i*4;
					if(mHasColor){
						buffer.putInt(p.x+up.x-right.x);buffer.putInt(p.y+up.y-right.y);buffer.putInt(p.z+up.z-right.z);
						//vba.set3(vi+0,0,	p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+0,1,	color);
						buffer.putInt(tx0);buffer.putInt(ty0);
						//vba.set2(vi+0,2,	tx0,ty0);

						buffer.putInt(p.x-up.x-right.x);buffer.putInt(p.y-up.y-right.y);buffer.putInt(p.z-up.z-right.z);
						//vba.set3(vi+1,0,	p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+1,1,	color);
						buffer.putInt(tx0);buffer.putInt(ty1);
						//vba.set2(vi+1,2,	tx0,ty1);

						buffer.putInt(p.x+up.x+right.x);buffer.putInt(p.y+up.y+right.y);buffer.putInt(p.z+up.z+right.z);
						//vba.set3(vi+2,0,	p.x+up.x+right.x, p.y+up.y+right.y, p.z+up.z+right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+2,1,	color);
						buffer.putInt(tx1);buffer.putInt(ty0);
						//vba.set2(vi+2,2,	tx1,ty0);

						buffer.putInt(p.x-up.x+right.x);buffer.putInt(p.y-up.y+right.y);buffer.putInt(p.z-up.z+right.z);
						//vba.set3(vi+3,0,	p.x-up.x+right.x, p.y-up.y+right.y, p.z-up.z+right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+3,1,	color);
						buffer.putInt(tx1);buffer.putInt(ty1);
						//vba.set2(vi+3,2,	tx1,ty1);
					}
					else{
						buffer.putInt(p.x+up.x-right.x);buffer.putInt(p.y+up.y-right.y);buffer.putInt(p.z+up.z-right.z);
						//vba.set3(vi+0,0,	p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
						buffer.putInt(tx0);buffer.putInt(ty0);
						//vba.set2(vi+0,1,	tx0,ty0);

						buffer.putInt(p.x-up.x-right.x);buffer.putInt(p.y-up.y-right.y);buffer.putInt(p.z-up.z-right.z);
						//vba.set3(vi+1,0,	p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
						buffer.putInt(tx0);buffer.putInt(ty1);
						//vba.set2(vi+1,1,	tx0,ty1);

						buffer.putInt(p.x+up.x+right.x);buffer.putInt(p.y+up.y+right.y);buffer.putInt(p.z+up.z+right.z);
						//vba.set3(vi+2,0,	p.x+up.x+right.x, p.y+up.y+right.y, p.z+up.z+right.z);
						buffer.putInt(tx1);buffer.putInt(ty0);
						//vba.set2(vi+2,1,	tx1,ty0);

						buffer.putInt(p.x-up.x+right.x);buffer.putInt(p.y-up.y+right.y);buffer.putInt(p.z-up.z+right.z);
						//vba.set3(vi+3,0,	p.x-up.x+right.x, p.y-up.y+right.y, p.z-up.z+right.z);
						buffer.putInt(tx1);buffer.putInt(ty1);
						//vba.set2(vi+3,1,	tx1,ty1);
					}
				}
			}
			else if(!mLineBeams){
				int vertexesPerBeam=((mParticlesPerBeam-2)*2+8);
				if(mHasColor){
					for(i=0;i<mNumParticles/mParticlesPerBeam;i++){
						int pi=i*mParticlesPerBeam;
						int vi=i*vertexesPerBeam;
						Particle p=mParticles[pi];
						Particle p1=mParticles[pi+1];
						Particle pn=mParticles[pi+mParticlesPerBeam-1];

						if(p.visible==false){
							color=0x00000000;
							if(mHasScale) scale=0;
						}
						else if(mIndividualAttributes){
							color=lerp_ccs(p.startColor,p.endColor,p.age);
							if(mHasScale) scale=lerp_sss(p.startScale,p.endScale,p.age);
						}
						else{
							color=lerp_ccs(startColor,endColor,p.age);
							if(mHasScale) scale=lerp_sss(startScale,endScale,p.age);
						}

						right.x=p1.x-p.x; right.y=p1.y-p.y; right.z=p1.z-p.z;

						normalizeCarefully(right,mEpsilon);
						cross_vvv(up,viewForward,right);
						if(normalizeCarefully(up,mEpsilon)==false){
							up.set(X_UNIT_VECTOR3);
						}
						mul_vs(up,scale);
						mul_vs(right,scale);


						buffer.putInt(p.x+up.x-right.x);buffer.putInt(p.y+up.y-right.y);buffer.putInt(p.z+up.z-right.z);
						//vba.set3(vi+0,0,		p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+0,1,		color);
						buffer.putInt(0);buffer.putInt(0);
						//vba.set2(vi+0,2,		0,0);

						buffer.putInt(p.x-up.x-right.x);buffer.putInt(p.y-up.y-right.y);buffer.putInt(p.z-up.z-right.z);
						//vba.set3(vi+1,0,		p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+1,1,		color);
						buffer.putInt(0);buffer.putInt(ONE);
						//vba.set2(vi+1,2,		0,ONE);

						buffer.putInt(p.x+up.x);buffer.putInt(p.y+up.y);buffer.putInt(p.z+up.z);
						//vba.set3(vi+2,0,		p.x+up.x, p.y+up.y, p.z+up.z);
						buffer.putInt(color);
						//vba.setABGR(vi+2,1,		color);
						buffer.putInt(QUARTER);buffer.putInt(0);
						//vba.set2(vi+2,2,		QUARTER,0);

						buffer.putInt(p.x-up.x);buffer.putInt(p.y-up.y);buffer.putInt(p.z-up.z);
						//vba.set3(vi+3,0,		p.x-up.x, p.y-up.y, p.z-up.z);
						buffer.putInt(color);
						//vba.setABGR(vi+3,1,		color);
						buffer.putInt(QUARTER);buffer.putInt(ONE);
						//vba.set2(vi+3,2,		QUARTER,ONE);


						for(j=0;j<mParticlesPerBeam-2;++j){
							int ipi=pi+j+1;
							int ivi=vi+j*2+4;
							Particle ip=mParticles[ipi];
							Particle ip1=mParticles[ipi+1];

							if(ip.visible==false){
								color=0x00000000;
								if(mHasScale) scale=0;
							}
							else if(mIndividualAttributes){
								color=lerp_ccs(ip.startColor,ip.endColor,ip.age);
								if(mHasScale) scale=lerp_sss(ip.startScale,ip.endScale,ip.age);
							}
							else{
								color=lerp_ccs(startColor,endColor,ip.age);
								if(mHasScale) scale=lerp_sss(startScale,endScale,ip.age);
							}

							right.x=ip1.x-ip.x; right.y=ip1.y-ip.y; right.z=ip1.z-ip.z;

							cross_vvv(up,viewForward,right);
							if(normalizeCarefully(up,mEpsilon)==false){
								up.set(X_UNIT_VECTOR3);
							}
							mul_vs(up,scale);

							buffer.putInt(ip.x+up.x);buffer.putInt(ip.y+up.y);buffer.putInt(ip.z+up.z);
							//vba.set3(ivi+0,0,		ip.x+up.x, ip.y+up.y, ip.z+up.z);
							buffer.putInt(color);
							//vba.setABGR(ivi+0,1,	color);
							buffer.putInt(QUARTER);buffer.putInt(0);
							//vba.set2(ivi+0,2,		QUARTER,0);

							buffer.putInt(ip.x-up.x);buffer.putInt(ip.y-up.y);buffer.putInt(ip.z-up.z);
							//vba.set3(ivi+1,0,		ip.x-up.x, ip.y-up.y, ip.z-up.z);
							buffer.putInt(color);
							//vba.setABGR(ivi+1,1,	color);
							buffer.putInt(QUARTER);buffer.putInt(ONE);
							//vba.set2(ivi+1,2,		QUARTER,ONE);
						}


						if(pn.visible==false){
							color=0x00000000;
							if(mHasScale) scale=0;
						}
						else if(mIndividualAttributes){
							color=lerp_ccs(pn.startColor,pn.endColor,pn.age);
							if(mHasScale) scale=lerp_sss(pn.startScale,pn.endScale,pn.age);
						}
						else{
							color=lerp_ccs(startColor,endColor,pn.age);
							if(mHasScale) scale=lerp_sss(startScale,endScale,pn.age);
						}

						// We re-use the up from the previous particle pair,
						//  since we always use particle(n) and particle(n+1), on the final beam cap we
						//  only need to calculate the right, and only if we drew middle particles
						if(j>0){
							normalizeCarefully(right,mEpsilon);
							mul_vs(right,scale);
						}

						vi=vi+vertexesPerBeam-4;
						buffer.putInt(pn.x+up.x);buffer.putInt(pn.y+up.y);buffer.putInt(pn.z+up.z);
						//vba.set3(vi+0,0,		pn.x+up.x, pn.y+up.y, pn.z+up.z);
						buffer.putInt(color);
						//vba.setABGR(vi+0,1,		color);
						buffer.putInt(THREE_QUARTERS);buffer.putInt(0);
						//vba.set2(vi+0,2,		THREE_QUARTERS,0);

						buffer.putInt(pn.x-up.x);buffer.putInt(pn.y-up.y);buffer.putInt(pn.z-up.z);
						//vba.set3(vi+1,0,		pn.x-up.x, pn.y-up.y, pn.z-up.z);
						buffer.putInt(color);
						//vba.setABGR(vi+1,1,		color);
						buffer.putInt(THREE_QUARTERS);buffer.putInt(ONE);
						//vba.set2(vi+1,2,		THREE_QUARTERS,ONE);

						buffer.putInt(pn.x+up.x+right.x);buffer.putInt(pn.y+up.y+right.y);buffer.putInt(pn.z+up.z+right.z);
						//vba.set3(vi+2,0,		pn.x+up.x+right.x, pn.y+up.y+right.y, pn.z+up.z+right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+2,1,		color);
						buffer.putInt(ONE);buffer.putInt(0);
						//vba.set2(vi+2,2,		ONE,0);

						buffer.putInt(pn.x-up.x+right.x);buffer.putInt(pn.y-up.y+right.y);buffer.putInt(pn.z-up.z+right.z);
						//vba.set3(vi+3,0,		pn.x-up.x+right.x, pn.y-up.y+right.y, pn.z-up.z+right.z);
						buffer.putInt(color);
						//vba.setABGR(vi+3,1,		color);
						buffer.putInt(ONE);buffer.putInt(ONE);
						//vba.set2(vi+3,2,		ONE,ONE);
					}
				}
				else{
					for(i=0;i<mNumParticles/mParticlesPerBeam;i++){
						int pi=i*mParticlesPerBeam;
						int vi=i*vertexesPerBeam;
						Particle p=mParticles[pi];
						Particle p1=mParticles[pi+1];
						Particle pn=mParticles[pi+mParticlesPerBeam-1];

						if(mHasScale){
							if(p.visible==false){
								scale=0;
							}
							else if(mIndividualAttributes){
								scale=lerp_sss(p.startScale,p.endScale,p.age);
							}
							else{
								scale=lerp_sss(startScale,endScale,p.age);
							}
						}

						right.x=p1.x-p.x; right.y=p1.y-p.y; right.z=p1.z-p.z;

						normalizeCarefully(right,mEpsilon);
						cross_vvv(up,viewForward,right);
						if(normalizeCarefully(up,mEpsilon)==false){
							up.set(X_UNIT_VECTOR3);
						}
						mul_vs(up,scale);
						mul_vs(right,scale);

						buffer.putInt(p.x+up.x-right.x);buffer.putInt(p.y+up.y-right.y);buffer.putInt(p.z+up.z-right.z);
						//vba.set3(vi+0,0,		p.x+up.x-right.x, p.y+up.y-right.y, p.z+up.z-right.z);
						buffer.putInt(0);buffer.putInt(0);
						//vba.set2(vi+0,2,		0,0);

						buffer.putInt(p.x-up.x-right.x);buffer.putInt(p.y-up.y-right.y);buffer.putInt(p.z-up.z-right.z);
						//vba.set3(vi+1,0,		p.x-up.x-right.x, p.y-up.y-right.y, p.z-up.z-right.z);
						buffer.putInt(0);buffer.putInt(ONE);
						//vba.set2(vi+1,2,		0,ONE);

						buffer.putInt(p.x+up.x);buffer.putInt(p.y+up.y);buffer.putInt(p.z+up.z);
						//vba.set3(vi+2,0,		p.x+up.x, p.y+up.y, p.z+up.z);
						buffer.putInt(QUARTER);buffer.putInt(0);
						//vba.set2(vi+2,2,		QUARTER,0);

						buffer.putInt(p.x-up.x);buffer.putInt(p.y-up.y);buffer.putInt(p.z-up.z);
						//vba.set3(vi+3,0,		p.x-up.x, p.y-up.y, p.z-up.z);
						buffer.putInt(QUARTER);buffer.putInt(ONE);
						//vba.set2(vi+3,2,		QUARTER,ONE);


						for(j=0;j<mParticlesPerBeam-2;++j){
							int ipi=pi+j+1;
							int ivi=vi+j*2+4;
							Particle ip=mParticles[ipi];
							Particle ip1=mParticles[ipi+1];

							if(mHasScale){
								if(ip.visible==false){
									scale=0;
								}
								else if(mIndividualAttributes){
									scale=lerp_sss(ip.startScale,ip.endScale,ip.age);
								}
								else{
									scale=lerp_sss(startScale,endScale,ip.age);
								}
							}

							right.x=ip1.x-ip.x; right.y=ip1.y-ip.y; right.z=ip1.z-ip.z;

							cross_vvv(up,viewForward,right);
							if(normalizeCarefully(up,mEpsilon)==false){
								up.set(X_UNIT_VECTOR3);
							}
							mul_vs(up,scale);

							buffer.putInt(ip.x+up.x);buffer.putInt(ip.y+up.y);buffer.putInt(ip.z+up.z);
							//vba.set3(ivi+0,0,		ip.x+up.x, ip.y+up.y, ip.z+up.z);
							buffer.putInt(QUARTER);buffer.putInt(0);
							//vba.set2(ivi+0,2,		QUARTER,0);

							buffer.putInt(ip.x-up.x);buffer.putInt(ip.y-up.y);buffer.putInt(ip.z-up.z);
							//vba.set3(ivi+1,0,		ip.x-up.x, ip.y-up.y, ip.z-up.z);
							buffer.putInt(QUARTER);buffer.putInt(ONE);
							//vba.set2(ivi+1,2,		QUARTER,ONE);
						}


						if(mHasScale){
							if(pn.visible==false){
								scale=0;
							}
							else if(mIndividualAttributes){
								scale=lerp_sss(pn.startScale,pn.endScale,pn.age);
							}
							else{
								scale=lerp_sss(startScale,endScale,pn.age);
							}
						}

						// We re-use the up from the previous particle pair,
						//  since we always use particle(n) and particle(n+1), on the final beam cap we
						//  only need to calculate the right, and only if we drew middle particles
						if(j>0){
							normalizeCarefully(right,mEpsilon);
							mul_vs(right,scale);
						}

						vi=vi+vertexesPerBeam-4;
						buffer.putInt(pn.x+up.x);buffer.putInt(pn.y+up.y);buffer.putInt(pn.z+up.z);
						//vba.set3(vi+0,0,		pn.x+up.x, pn.y+up.y, pn.z+up.z);
						buffer.putInt(THREE_QUARTERS);buffer.putInt(0);
						//vba.set2(vi+0,2,		THREE_QUARTERS,0);

						buffer.putInt(pn.x-up.x);buffer.putInt(pn.y-up.y);buffer.putInt(pn.z-up.z);
						//vba.set3(vi+1,0,		pn.x-up.x, pn.y-up.y, pn.z-up.z);
						buffer.putInt(THREE_QUARTERS);buffer.putInt(ONE);
						//vba.set2(vi+1,2,		THREE_QUARTERS,ONE);

						buffer.putInt(pn.x+up.x+right.x);buffer.putInt(pn.y+up.y+right.y);buffer.putInt(pn.z+up.z+right.z);
						//vba.set3(vi+2,0,		pn.x+up.x+right.x, pn.y+up.y+right.y, pn.z+up.z+right.z);
						buffer.putInt(ONE);buffer.putInt(0);
						//vba.set2(vi+2,2,		ONE,0);

						buffer.putInt(pn.x-up.x+right.x);buffer.putInt(pn.y-up.y+right.y);buffer.putInt(pn.z-up.z+right.z);
						//vba.set3(vi+3,0,		pn.x-up.x+right.x, pn.y-up.y+right.y, pn.z-up.z+right.z);
						buffer.putInt(ONE);buffer.putInt(ONE);
						//vba.set2(vi+3,2,		ONE,ONE);
					}
				}
			}
			else{
				for(i=0;i<mNumParticles;i++){
					Particle p=mParticles[i];
					if(p.visible==false){
						color=0x00000000;
					}
					else if(mIndividualAttributes){
						color=lerp_ccs(p.startColor,p.endColor,p.age);
					}
					else{
						color=lerp_ccs(startColor,endColor,p.age);
					}

					// Optimized to write to buffer directly for speed
					//vba.set3(i,0,p.x,p.y,p.z);
					buffer.putInt(p.x);buffer.putInt(p.y);buffer.putInt(p.z);
					if(mHasColor){
						//vba.setABGR(i,1,color);
						buffer.putInt(color);
						//vba.set2(i,2,HALF,HALF);
						buffer.putInt(HALF);buffer.putInt(HALF);
					}
					else{
						//vba.set2(i,1,HALF,HALF);
						buffer.putInt(HALF);buffer.putInt(HALF);
					}
				}
			}

			buffer.rewind();
			vba.unlock();
		}
	}

	protected boolean mDestroyNextLogicFrame=false;

	protected Random mRandom=new Random();

	protected boolean mWorldSpace=false;
	protected boolean mOrientation=false;
	protected boolean mIndividualAttributes=false;
	protected int mParticlesPerBeam=0;
	protected boolean mDestroyOnFinish=false;
	protected boolean mHadPoints=false;
	protected boolean mHadAges=false;
	protected boolean mHasColor=false;
	protected boolean mHasScale=false;
	protected boolean mLineBeams=false;

	protected Particle[] mParticles=new Particle[0];
	protected int mNumParticles=0;

	protected boolean mRandomStartColor=false;
	protected Color mStartColor=new Color();
	protected Color mEndColor=new Color();
	protected scalar mStartScale=0;
	protected scalar mEndScale=0;

	protected TextureSection mTextureSection=null;
	protected Material mMaterial=null;
	protected VertexBuffer mVertexBuffer=null;
	protected VertexData mVertexData=null;
	protected IndexBuffer mIndexBuffer=null;
	protected IndexData mIndexData=null;
	protected IndexData mLineIndexData=null;

	protected Matrix4x4 mWorldTransform=new Matrix4x4();

	protected boolean mUpdateParticles=false;
	protected ParticleSimulator mParticleSimulator=null;

	protected boolean mHasIdealViewTransform=false;
	protected Matrix4x4 mIdealViewTransform=new Matrix4x4();

	protected scalar mEpsilon=0;

	protected Vector3 cache_updateVertexBuffer_viewUp=new Vector3();
	protected Vector3 cache_updateVertexBuffer_viewRight=new Vector3();
	protected Vector3 cache_updateVertexBuffer_viewForward=new Vector3();
	protected Vector3 cache_updateVertexBuffer_up=new Vector3();
	protected Vector3 cache_updateVertexBuffer_right=new Vector3();
	protected Vector3 cache_updateVertexBuffer_forward=new Vector3();
	protected Matrix4x4 cache_updateWorldTransform_temp1=new Matrix4x4();
	protected Matrix4x4 cache_updateWorldTransform_temp2=new Matrix4x4();
}
