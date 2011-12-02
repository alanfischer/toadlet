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

package com.lightningtoads.toadlet.tadpole.millihop;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.sg.*;
//#ifdef TOADLET_PLATFORM_JSR184
	import javax.microedition.lcdui.*;
	import javax.microedition.m3g.*;
//#endif
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

//#define TOADLET_NO_USE_CLAMP

public final class HopSceneManager extends SceneManager{
	public HopSceneManager(Engine engine){
		super(engine);

		mFixedDT=50; // Fixed DT of 20 fps
		mAccumulatedDT=0;

		simulatorConstruct();
	}

	public void update(int dt){
		super.update(dt);

		int i;
		if(mFixedDT==0){
			//#ifdef TOADLET_DEBUG
				Profile.beginSection(Profile.SECTION_PHYSICS);
			//#endif

			for(i=mNumHopEntities-1;i>=0;--i){
				HopEntity entity=mHopEntities[i];
				entity.prePhysicsNoFixedDT();
			}

			simulatorUpdate(dt);

			for(i=mNumHopEntities-1;i>=0;--i){
				HopEntity entity=mHopEntities[i];
				entity.postPhysicsNoFixedDT();
				entity.updateThink(dt);
			}

			//#ifdef TOADLET_DEBUG
				Profile.endSection(Profile.SECTION_PHYSICS);
			//#endif
		}
		else{
			mAccumulatedDT+=dt;

			if(mAccumulatedDT>=mFixedDT){
				for(i=mNumHopEntities-1;i>=0;--i){
					HopEntity entity=mHopEntities[i];
					entity.prePhysicsFixedDT();
				}

				while(mAccumulatedDT>=mFixedDT){
					mAccumulatedDT-=mFixedDT;
					simulatorUpdate(mFixedDT);

					for(i=mNumHopEntities-1;i>=0;--i){
						HopEntity entity=mHopEntities[i];
						entity.updateThink(mFixedDT);
					}
				}
			}

			int f=Math.div(Math.intToFixed(mAccumulatedDT),Math.intToFixed(mFixedDT));

			for(i=mNumHopEntities-1;i>=0;--i){
				HopEntity entity=mHopEntities[i];
				entity.interpolatePhysicalParameters(f);
			}
		}
	}

	//#ifdef TOADLET_PLATFORM_JSR184
		public final void render(Graphics g,CameraEntity camera){
			if(mShadowVertexBuffer!=null){
				int i;
				for(i=mNumHopEntities-1;i>=0;--i){
					HopEntity entity=mHopEntities[i];
					if(entity.mCastsShadow){
						mInterpolationCache1.set(entity.getTranslate());
						mInterpolationCache1.z-=mShadowTestLength;
						traceLine(entity.getTranslate(),mInterpolationCache1,entity,mShadowTraceCache);
						if(mShadowTraceCache.solid!=null && mShadowTraceCache.solid.hasInfiniteMass()){
							Mesh mesh=entity.mShadowMesh;
							Vector3 point=mShadowTraceCache.point;
							Vector3 normal=mShadowTraceCache.normal;

							mesh.setTranslation(Math.fixedToFloat(point.x),Math.fixedToFloat(point.y),Math.fixedToFloat(point.z+mShadowOffset));
							mesh.setRenderingEnable(true);

							if(normal.equals(Math.Z_UNIT_VECTOR3)){
								mesh.setOrientation(0,0,0,0);
							}
							else if(normal.equals(Math.NEG_Z_UNIT_VECTOR3)){
								mesh.setOrientation(180,1,0,0);
							}
							else{
								// TODO: Optimize this case
								Math.cross(mInterpolationCache1,Math.NEG_Z_UNIT_VECTOR3,normal);
								int length=Math.length(mInterpolationCache1);
								int degrees=Math.radToDeg(Math.arcsin(length));

								if(length>0){
									length=Math.div(Math.ONE,length);
								}

								mInterpolationCache1.x=Math.mul(mInterpolationCache1.x,length);
								mInterpolationCache1.y=Math.mul(mInterpolationCache1.y,length);
								mInterpolationCache1.z=Math.mul(mInterpolationCache1.z,length);

								if(mInterpolationCache1.equals(Math.ZERO_VECTOR3)){
									degrees=0;
								}
								else{
									degrees=-degrees;
								}

								mesh.setOrientation(Math.fixedToFloat(degrees),
									Math.fixedToFloat(mInterpolationCache1.x),Math.fixedToFloat(mInterpolationCache1.y),Math.fixedToFloat(mInterpolationCache1.z));
							}
						}
						else{
							entity.mShadowMesh.setRenderingEnable(false);
						}
					}
					else{
						entity.mShadowMesh.setRenderingEnable(false);
					}
				}
			}

			super.render(g,camera);
		}
	//#endif

	//#ifdef TOADLET_PLATFORM_JSR184
		public void setShadows(String textureName,boolean shadows,int scale,int testLength,int offset,int layer){
			mShadowScale=scale;
			mShadowTestLength=testLength;
			mShadowOffset=offset;
			mShadowLayer=layer;

			if(shadows && mShadowVertexBuffer==null){
				try{
					VertexArray positions=new VertexArray(4,3,1);
					{
						byte[] values=new byte[12];
						values[0]=-1;
						values[1]=-1;
						values[2]=0;
						values[3]=1;
						values[4]=-1;
						values[5]=0;
						values[6]=-1;
						values[7]=1;
						values[8]=0;
						values[9]=1;
						values[10]=1;
						values[11]=0;
						positions.set(0,4,values);
					}
					VertexArray texCoords=new VertexArray(4,2,1);
					{
						byte[] values=new byte[8];
						values[0]=1;
						values[1]=1;
						values[2]=1;
						values[3]=0;
						values[4]=0;
						values[5]=1;
						values[6]=0;
						values[7]=0;
						texCoords.set(0,4,values);
					}
					mShadowVertexBuffer=new VertexBuffer();
					{
						mShadowVertexBuffer.setPositions(positions,Math.fixedToFloat(scale),null);
						mShadowVertexBuffer.setTexCoords(0,texCoords,1.0f,null);
					}

					mShadowIndexBuffer=new TriangleStripArray(0,new int[]{4});

					mShadowAppearance=new Appearance();
					{
						CompositingMode compositingMode=new CompositingMode();
						compositingMode.setDepthWriteEnable(false);
						compositingMode.setBlending(CompositingMode.MODULATE);
						mShadowAppearance.setCompositingMode(compositingMode);
						mShadowAppearance.setLayer(layer);
						Image2D image=new Image2D(Image2D.RGBA,Image.createImage(textureName+".png"));
						mShadowAppearance.setTexture(0,new Texture2D(image));
					}

					int i;
					for(i=0;i<mNumHopEntities;++i){
						HopEntity entity=mHopEntities[i];
						entity.mShadowMesh=new Mesh(mShadowVertexBuffer,mShadowIndexBuffer,mShadowAppearance);
						entity.mShadowMesh.setRenderingEnable(false);
						getWorld().addChild(entity.mShadowMesh);
					}
				}
				catch(Exception ex){
					//#ifdef TOADLET_DEBUG
						ex.printStackTrace();
					//#endif
				}
			}
			else if(mShadowVertexBuffer!=null){
				mShadowAppearance=null;
				mShadowIndexBuffer=null;
				mShadowVertexBuffer=null;

				int i;
				for(i=0;i<mNumHopEntities;++i){
					HopEntity entity=mHopEntities[i];
					getWorld().removeChild(entity.mShadowMesh);
					entity.mShadowMesh=null;
				}
			}
		}
	//#endif

	void registerHopEntity(HopEntity entity){
		int i;
		for(i=0;i<mNumHopEntities;++i){
			if(mHopEntities[i]==entity)break;
		}

		if(i==mNumHopEntities){
			if(mHopEntities.length<mNumHopEntities+1){
				HopEntity[] entities=new HopEntity[mNumHopEntities+1];
				System.arraycopy(mHopEntities,0,entities,0,mHopEntities.length);
				mHopEntities=entities;
			}

			mHopEntities[mNumHopEntities]=entity;
			mNumHopEntities++;
		}

		//#ifdef TOADLET_PLATFORM_JSR184
			if(mShadowVertexBuffer!=null){
				entity.mShadowMesh=new Mesh(mShadowVertexBuffer,mShadowIndexBuffer,mShadowAppearance);
				entity.mShadowMesh.setRenderingEnable(false);
				getWorld().addChild(entity.mShadowMesh);
			}
		//#endif
	}

	void unregisterHopEntity(HopEntity entity){
		int i;
		for(i=0;i<mNumHopEntities;++i){
			if(mHopEntities[i]==entity)break;
		}
		if(i!=mNumHopEntities){
			mHopEntities[i]=null;
			System.arraycopy(mHopEntities,i+1,mHopEntities,i,mNumHopEntities-(i+1));
			mNumHopEntities--;
			mHopEntities[mNumHopEntities]=null;
		}

		//#ifdef TOADLET_PLATFORM_JSR184
			if(mShadowVertexBuffer!=null){
				getWorld().removeChild(entity.mShadowMesh);
				entity.mShadowMesh=null;
			}
		//#endif
	}

	// A fixed DT of 0 disabled fixed DTs
	public void setFixedDT(int dt){
		mFixedDT=dt;
	}

	public int getFixedDT(){
		return mFixedDT;
	}

	HopEntity[] mHopEntities=new HopEntity[0];
	int mNumHopEntities;
	int mFixedDT;
	int mAccumulatedDT;
	Vector3 mInterpolationCache1=new Vector3();
	Vector3 mInterpolationCache2=new Vector3();
	//#ifdef TOADLET_PLATFORM_JSR184
		VertexBuffer mShadowVertexBuffer=null;
		IndexBuffer mShadowIndexBuffer=null;
		Appearance mShadowAppearance=null;
		Collision mShadowTraceCache=new Collision();
		int mShadowScale=Math.ONE;
		int mShadowTestLength=0;
		int mShadowOffset=0;
		int mShadowLayer=1;
	//#endif





	public final static byte IT_EULER=0;
	public final static byte IT_IMPROVED=1;
	public final static byte IT_RUNGE_KUTTA=2;

	public void simulatorConstruct(){
		setEpsilonBits(4);
		setMaxVelocityComponent(Math.intToFixed(180)); // Above 180 we can overflow in square
		setGravity(new Vector3(0,0,-Math.milliToFixed(9810)));
		setIntegratorType(IT_EULER);
		setDeactivateSpeed(1<<8); // Some default
		setDeactivateCount(4); // 4 frames
	}

	public void setEpsilonBits(int epsilonBits){
		mEpsilonBits=epsilonBits;
		mEpsilon=1<<mEpsilonBits;
		mHalfEpsilon=mEpsilon>>1;
		mQuarterEpsilon=mEpsilon>>2;
	}
	public int getEpsilonBits(){return mEpsilonBits;}

	public void setIntegratorType(byte integratorType){mIntegratorType=integratorType;}
	public byte getIntegratorType(){return mIntegratorType;}
	
	public void setMaxVelocityComponent(int maxVelocityComponent){mMaxVelocityComponent=maxVelocityComponent;}
	public int getMaxVelocityComponent(){return mMaxVelocityComponent;}
	
	public void setGravity(Vector3 gravity){
		mGravity.set(gravity);
		int i;
		for(i=0;i<mNumSolids;++i){
			mSolids[i].mActive=true;
			mSolids[i].mDeactivateCount=0;
		}
	}
	public Vector3 getGravity(){return mGravity;}

	public void setFluidVelocity(Vector3 fluidVelocity){mFluidVelocity.set(fluidVelocity);}
	public Vector3 getFluidVelocity(){return mFluidVelocity;}

	public void setManager(Manager manager){mManager=manager;}
	public Manager getManager(){return mManager;}

	public void setDeactivateSpeed(int speed){mDeactivateSpeed=speed;}
	public void setDeactivateCount(int count){mDeactivateCount=count;}

	public void addSolid(HopEntity solid){
		int i;
		for(i=0;i<mNumSolids;++i){
			if(mSolids[i]==solid)break;
		}

		if(i==mNumSolids){
			if(mSolids.length<mNumSolids+1){
				HopEntity [] solids=new HopEntity [mNumSolids+1];
				System.arraycopy(mSolids,0,solids,0,mSolids.length);
				mSolids=solids;

				mSpacialCollection=new HopEntity [mSolids.length];
			}

			mSolids[mNumSolids]=solid;
			mNumSolids++;

			solid.mActive=true;
			solid.mDeactivateCount=0;
		}
	}

	public void removeSolid(HopEntity solid){
		// We can't touch anything if we're not in the simulation
		// This will help in some logic code
		solid.mTouching=null;
		solid.mLastTouched=null;

		int i;
		for(i=0;i<mNumSolids;++i){
			HopEntity s=mSolids[i];
			if(s.mLastTouched==solid){
				s.mLastTouched=null;
			}
			if(s.mTouching==solid){
				s.mTouching=null;
			}
		}

		for(i=0;i<mNumSolids;++i){
			if(mSolids[i]==solid)break;
		}
		if(i!=mNumSolids){
			mSolids[i]=null;
			System.arraycopy(mSolids,i+1,mSolids,i,mNumSolids-(i+1));
			mNumSolids--;
			mSolids[mNumSolids]=null;
		}
	}

	public int getNumSolids(){return mNumSolids;}
	public HopEntity getSolid(int i){return mSolids[i];}
	
	public void simulatorUpdate(int dt){
		Vector3 oldPosition=mCacheOldPosition;oldPosition.reset();
		Vector3 newPosition=mCacheNewPosition;newPosition.reset();
		Vector3 offset=mCacheOffset;offset.reset();
		Vector3 velocity=mCacheVelocity;velocity.reset();
		Vector3 temp=mCacheTemp;temp.reset();
		Vector3 t=mCacheT;t.reset();
		Vector3 leftOver=mCacheLeftOver;leftOver.reset();
		Vector3 frictionForce=mCacheFrictionForce;frictionForce.reset();
		Vector3 k1=mCacheK1;k1.reset();
		Vector3 k2=mCacheK2;k2.reset();
		Vector3 k3=mCacheK3;k3.reset();
		Vector3 k4=mCacheK4;k4.reset();
		int cor=0,j=0,oneOverMass=0,oneOverHitMass=0;
		int loop=0;
		HopEntity solid=null,hitSolid=null;
		Collision c=mCacheUpdateCollision;c.reset();
		int numCollisions=0;
		int i;

		// Convert millisecond dt to fixed point dt
		int fdt=Math.milliToFixed(dt);

		if(mManager!=null){
			mManager.preUpdate(dt,fdt);
		}

		for(i=0;i<mNumSolids;i++){
			solid=mSolids[i];

			if(solid.mActive==false){
				continue;
			}

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.preUpdate(solid,dt,fdt);
			}
			
			hitSolid=solid.mTouching;
			frictionLink(solid,solid.mVelocity,hitSolid,fdt,frictionForce);

			if(solid.mMass>0){
				oneOverMass=
					//#exec fpcomp.bat "(1/solid.mMass)"
					;
			}
			else{
				oneOverMass=0;
			}

			// Integrate to obtain velocity
			if(mIntegratorType==IT_EULER){
				Math.mul(t,mGravity,
					//#exec fpcomp.bat "solid.mCoefficientOfGravity*fdt"
					);
				Math.add(velocity,solid.mVelocity,t);
				if(solid.mMass!=0){
					Math.sub(t,solid.mVelocity,mFluidVelocity);
					Math.mul(t,solid.mCoefficientOfEffectiveDrag);
					Math.sub(t,solid.mForce,t);
					Math.add(t,frictionForce);
					Math.mul(t,
						//#exec fpcomp.bat "(fdt*oneOverMass)"
						);
					Math.add(velocity,t);
				}
			}
			else if(mIntegratorType==IT_IMPROVED){
				Math.mul(k1,mGravity,
					//#exec fpcomp.bat "solid.mCoefficientOfGravity*fdt"
					);
				k2.set(k1);
				if(solid.mMass!=0){
					// Calculate relative velocity and store in leftOver
					Math.sub(leftOver,solid.mVelocity,mFluidVelocity);

					Math.mul(t,leftOver,solid.mCoefficientOfEffectiveDrag);
					Math.sub(t,solid.mForce,t);
					Math.add(t,frictionForce);
					Math.mul(t,
						//#exec fpcomp.bat "(fdt*oneOverMass)"
						);
					Math.add(k1,t);
					
					temp.set(frictionForce);
					
					Math.add(t,solid.mVelocity,k1);
					frictionLink(solid,t,hitSolid,fdt,frictionForce);
					
					Math.add(t,leftOver,k1);
					Math.mul(t,solid.mCoefficientOfEffectiveDrag);
					Math.sub(t,solid.mForce,t);
					Math.add(t,frictionForce);
					Math.mul(t,
						//#exec fpcomp.bat "(fdt*oneOverMass)"
						);
					Math.add(k1,t);
					
					Math.add(frictionForce,temp);
					Math.mul(frictionForce,Math.HALF);
				}
				Math.add(t,k1,k2);
				Math.mul(t,Math.HALF);
				Math.add(velocity,solid.mVelocity,t);
			}
			else if(mIntegratorType==IT_RUNGE_KUTTA){
				throw new RuntimeException("IT_RUNGE_KUTTA not implemented");
			}

			if(solid.mInteraction==HopEntity.INTERACTION_NORMAL && hitSolid!=null && hitSolid.hasInfiniteMass()==false){
				Math.mul(temp,frictionForce,-Math.ONE);
				hitSolid.addForce(temp);
			}

			// Cap velocity
			if(velocity.x>mMaxVelocityComponent){velocity.x=mMaxVelocityComponent;}
			if(velocity.x<-mMaxVelocityComponent){velocity.x=-mMaxVelocityComponent;}
			if(velocity.y>mMaxVelocityComponent){velocity.y=mMaxVelocityComponent;}
			if(velocity.y<-mMaxVelocityComponent){velocity.y=-mMaxVelocityComponent;}
			if(velocity.z>mMaxVelocityComponent){velocity.z=mMaxVelocityComponent;}
			if(velocity.z<-mMaxVelocityComponent){velocity.z=-mMaxVelocityComponent;}

			// Integrate to obtain positions
			oldPosition.set(solid.mPosition);
			if(mIntegratorType==IT_EULER){
				Math.mul(temp,velocity,fdt);
				Math.add(newPosition,oldPosition,temp);
			}
			else if(mIntegratorType==IT_IMPROVED || mIntegratorType==IT_RUNGE_KUTTA){
				// The Runge Kutta denegerates to the Improved for position
				Math.mul(k1,solid.mVelocity,fdt);
				Math.mul(k2,velocity,fdt);
				Math.add(temp,k1,k2);
				Math.mul(temp,Math.HALF);
				Math.add(newPosition,oldPosition,temp);
			}

			// Clean up
			solid.mVelocity.set(velocity);
			solid.clearForce();

			boolean first=true;
			boolean skip=false;

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.intraUpdate(solid,dt,fdt);
			}

			// Collect all possible solids in the whole movement area
			if(solid.mInteraction!=HopEntity.INTERACTION_NONE){
				//#ifdef TOADLET_USE_CLAMP
					clampPosition(oldPosition);
					clampPosition(newPosition);
				//#endif

				Math.sub(temp,newPosition,oldPosition);
				if(toSmall(temp)){
					newPosition.set(oldPosition);
					skip=true;
				}
				else{
					if(temp.x<0){temp.x=-temp.x;}
					if(temp.y<0){temp.y=-temp.y;}
					if(temp.z<0){temp.z=-temp.z;}

					int m=temp.x;
					if(temp.y>m){m=temp.y;}
					if(temp.z>m){m=temp.z;}
					m+=mEpsilon; // Move it out by epsilon, since we haven't clamped yet

					//AABox startBox=new AABox(solid.mLocalBound);
					AABox box=mCacheBox1;
					box.set(solid.mLocalBound);

					Math.add(box,newPosition);
					box.mins.x-=m;
					box.mins.y-=m;
					box.mins.z-=m;
					box.maxs.x+=m;
					box.maxs.y+=m;
					box.maxs.z+=m;

					mNumSpacialCollection=getSolidsInAABox(box,mSpacialCollection);
				}
			}

			// Loop to use up available momentum
			loop=0;
			while(skip==false){
				if(first==true){
					first=false;
				}
				else{
					//#ifdef TOADLET_USE_CLAMP
						clampPosition(oldPosition);
						clampPosition(newPosition);
					//#endif

					Math.sub(temp,newPosition,oldPosition);
					if(toSmall(temp)){
						newPosition.set(oldPosition);
						break;
					}
				}
				traceSolid(solid,oldPosition,newPosition,c);

				if(c.time>=0){
					// TODO: Improve this Inside logic
					if(c.time==0 && c.solid!=null && c.solid.mMass==HopEntity.INFINITE_MASS){
						solid.mInside=true;
					}
					else{
						solid.mInside=false;
					}

					// If its a valid collision, and someone is listening, then store it
					if(c.solid!=solid.mTouching &&
						(solid.mCollisionListener!=null || c.solid.mCollisionListener!=null)){

						c.collider=solid;
						Math.sub(c.velocity,solid.mVelocity,c.solid.mVelocity);

						if(mCollisions.length<=numCollisions){
							Collision[] collisions=new Collision[numCollisions+1];
							System.arraycopy(mCollisions,0,collisions,0,mCollisions.length);
							collisions[collisions.length-1]=new Collision();
							mCollisions=collisions;
						}
						
						mCollisions[numCollisions].set(c);
						numCollisions++;
					}
					hitSolid=c.solid;
					
					// Conservation of momentum
					if(solid.mCoefficientOfRestitutionOverride){
						cor=solid.mCoefficientOfRestitution;
					}
					else{
						cor=solid.mCoefficientOfRestitution+hitSolid.mCoefficientOfRestitution>>1;
					}

					Math.sub(temp,hitSolid.mVelocity,solid.mVelocity);
					int numerator=
						//#exec fpcomp.bat "((1+cor) * Math.dot(temp,c.normal))"
						;

					// Temp stores the velocity change on hitSolid
					temp.reset();

					if(solid.mMass!=0 && hitSolid.mMass!=0){
						if(solid.hasInfiniteMass()){
							oneOverMass=0;
						}
						else{
							oneOverMass=
								//#exec fpcomp.bat "1/solid.mMass"
								;
						}
					
						if(hitSolid.hasInfiniteMass()){
							oneOverHitMass=0;
						}
						else{
							oneOverHitMass=
								//#exec fpcomp.bat "1/hitSolid.mMass"
								;
						}

						if(oneOverMass+oneOverHitMass!=0){
							j=
								//#exec fpcomp.bat "numerator/(oneOverMass+oneOverHitMass)"
								;
						}
						else{
							j=0;
						}

						if(solid.hasInfiniteMass()==false){
							Math.mul(t,c.normal,j);
							Math.mul(t,oneOverMass);
							Math.add(solid.mVelocity,t);
						}
						if(hitSolid.hasInfiniteMass()==false && solid.mInteraction==HopEntity.INTERACTION_NORMAL){
							if(solid.hasInfiniteMass()){
								// HACK: For now, infinite mass trains assume a CoefficientOfRestitution of 1 
								// for the hitSolid calculation to avoid having gravity jam objects against them 
								Math.sub(temp,hitSolid.mVelocity,solid.mVelocity);
								Math.mul(temp,c.normal,Math.dot(temp,c.normal)<<1);
							}
							else{
								Math.mul(temp,c.normal,j);
								Math.mul(temp,oneOverHitMass);
							}
						}
					}
					else if(hitSolid.mMass==0 && solid.mInteraction==HopEntity.INTERACTION_NORMAL){
						Math.mul(temp,c.normal,numerator);
					}
					else if(solid.mMass==0){
						Math.mul(t,c.normal,numerator);
						Math.add(solid.mVelocity,t);
					}

					if(Math.abs(temp.x)>=mDeactivateSpeed || Math.abs(temp.y)>=mDeactivateSpeed || Math.abs(temp.z)>=mDeactivateSpeed){
						hitSolid.mActive=true;
						hitSolid.mDeactivateCount=0;
						Math.sub(hitSolid.mVelocity,temp);
					}

					// Calculate offset vector, and then resulting position
					//#ifdef TOADLET_USE_CLAMP
						clampPosition(c.point);
					//#endif

					// Different offset options
					//#define TOADLET_OFFSET_1
					//#ifdef TOADLET_OFFSET_1
						temp.set(c.normal);
						convertToEpsilonOffset(temp);
						Math.add(temp,c.point);
					//#elifdef TOADLET_OFFSET_2
						Math.sub(offset,oldPosition,c.point);
						convertToEpsilonOffset(offset);
						temp.set(offset);
						Math.add(temp,c.point);
					//#elifdef TOADLET_OFFSET_3
						Math.sub(offset,oldPosition,c.point);
						convertToEpsilonOffset(offset);
						temp.set(c.normal);
						convertToEpsilonOffset(temp);
						Math.add(temp,c.point);
						Math.add(temp,offset);
					//#else
						temp.set(c.point);
					//#endif

					// Calculate left over amount
					Math.sub(leftOver,newPosition,temp);

					if(solid.mInteraction!=HopEntity.INTERACTION_NONE && c.solid.mInteraction==HopEntity.INTERACTION_NORMAL){
						// Touching code
						if(solid.mLastTouched==c.solid){
							solid.mTouching=c.solid;
							solid.mTouchingNormal.set(c.normal);
						}
						else{
							solid.mLastTouched=c.solid;
							solid.mTouching=null;
						}
					}

					if(toSmall(leftOver)){
						newPosition.set(temp);
						break;
					}
					else if(loop>4){
						newPosition.set(temp);
						break;
					}
					else{
						// Calculate new destinations from coefficient of restitution applied to velocity
						if(Math.normalizeCarefully(velocity,solid.mVelocity,mEpsilon)==false){
							newPosition.set(temp);
							break;
						}
						else{
							oldPosition.set(temp);
							Math.mul(velocity,Math.length(leftOver));
							Math.mul(temp,c.normal,Math.dot(velocity,c.normal));
							Math.sub(velocity,temp);
							Math.add(newPosition,oldPosition,velocity);
						}
					}
				}
				else{
					break;
				}

				loop++;
			}

			// Touching code
			if(c.time<0 && loop==0){
				if(solid.mTouching!=null && solid.mInteraction!=HopEntity.INTERACTION_NONE && solid.mTouching.mInteraction==HopEntity.INTERACTION_NORMAL){
					solid.mLastTouched=solid.mTouching;
					solid.mTouching=null;
				}
				else{
					solid.mLastTouched=null;
				}
			}

			if(Math.abs(newPosition.x-solid.mPosition.x)<mDeactivateSpeed && Math.abs(newPosition.y-solid.mPosition.y)<mDeactivateSpeed && Math.abs(newPosition.z-solid.mPosition.z)<mDeactivateSpeed){
				solid.mDeactivateCount++;
				if(solid.mDeactivateCount>mDeactivateCount){
					solid.mActive=false;
					solid.mDeactivateCount=0;
				}
			}
			else{
				solid.mDeactivateCount=0;
			}

			solid.setPositionNoActivate(newPosition);

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.postUpdate(solid,dt,fdt);
			}
		}

		// Now report all collisions to both solids
		for(i=0;i<numCollisions;++i){
			Collision col=mCollisions[i];

			CollisionListener listener=col.collider.mCollisionListener;
			if(listener!=null){
				listener.collision(col);
			}

			HopEntity s=col.solid;
			col.solid=col.collider;
			col.collider=s;

			listener=col.collider.mCollisionListener;
			if(col.collider.mInteraction==HopEntity.INTERACTION_NORMAL && listener!=null){
				Math.neg(col.normal);
				Math.neg(col.velocity);
				listener.collision(col);
			}
		}

		if(mManager!=null){
			mManager.postUpdate(dt,fdt);
		}
	}

	public int getSolidsInAABox(AABox box,HopEntity[] solids){
		int amount=0;

		int i;
		for(i=0;i<mNumSolids;++i){
			if(Math.testIntersection(box,mSolids[i].mWorldBound)){
				solids[amount]=mSolids[i];
				amount++;
			}
		}

		return amount;
	}

	public void getHopEntitiesInAABox(AABox box,java.util.Vector solids){
		int amount=getSolidsInAABox(box,mSpacialCollection);
		solids.setSize(amount);

		int i;
		for(i=0;i<amount;++i){
			solids.setElementAt(mSpacialCollection[i],i);
		}
	}
	
	public void convertToEpsilonOffset(Vector3 offset){
		if(offset.x>=mQuarterEpsilon)offset.x=mEpsilon;
		else if(offset.x<=-mQuarterEpsilon)offset.x=-mEpsilon;
		else offset.x=0;

		if(offset.y>=mQuarterEpsilon)offset.y=mEpsilon;
		else if(offset.y<=-mQuarterEpsilon)offset.y=-mEpsilon;
		else offset.y=0;

		if(offset.z>=mQuarterEpsilon)offset.z=mEpsilon;
		else if(offset.z<=-mQuarterEpsilon)offset.z=-mEpsilon;
		else offset.z=0;
	}
	
	public void convertToGenerousOffset(Vector3 offset){
		if(offset.x>=0)offset.x=mEpsilon;
		else if(offset.x<=0)offset.x=-mEpsilon;
		else offset.x=0;

		if(offset.y>=0)offset.y=mEpsilon;
		else if(offset.y<=-0)offset.y=-mEpsilon;
		else offset.y=0;

		if(offset.z>=0)offset.z=mEpsilon;
		else if(offset.z<=-0)offset.z=-mEpsilon;
		else offset.z=0;
	}

	public void clampPosition(Vector3 pos){
		//#ifdef TOADLET_USE_CLAMP
			pos.x = ((pos.x + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits;
			pos.y = ((pos.y + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits;
			pos.z = ((pos.z + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits;
		//#endif
	}
	
	public boolean toSmall(Vector3 value){
		return (value.x<mEpsilon && value.x>-mEpsilon && value.y<mEpsilon && value.y>-mEpsilon && value.z<mEpsilon && value.z>-mEpsilon);
	}

	public void traceLine(Vector3 start,Vector3 end,HopEntity ignore,Collision closest){
		closest.time=-Math.ONE;

		Segment s=mCacheSegment1;s.set(start,end);

		HopEntity solid2;

		//#ifdef TOADLET_USE_CLAMP
			clampPosition(s.origin);
			clampPosition(s.direction);
		//#endif

		AABox total=mCacheBox3;total.set(start,start);
		
		total.mergeWith(end);
		mNumSpacialCollection=getSolidsInAABox(total,mSpacialCollection);
		
		int i;
		for(i=0;i<mNumSpacialCollection;++i){
			solid2=mSpacialCollection[i];
			if(ignore!=solid2 && solid2.mInteraction==HopEntity.INTERACTION_NORMAL){
				mCacheCollision1.time=-Math.ONE;
				testLine(solid2,s,mCacheCollision1);
				if(mCacheCollision1.time>=0){
					if(closest.time<0 || mCacheCollision1.time<closest.time){
						closest.set(mCacheCollision1);
					}
					else if(closest.time==mCacheCollision1.time){
						Math.add(closest.normal,mCacheCollision1.normal);
						boolean b=Math.normalizeCarefully(closest.normal,mEpsilon);
						if(b==false){
							closest.set(mCacheCollision1);
						}
					}
				}

			}
		}

		if(mManager!=null){
			mCacheCollision1.time=-Math.ONE;
			mManager.traceLine(s,mCacheCollision1);
			if(mCacheCollision1.time>=0){
				if(closest.time<0 || mCacheCollision1.time<closest.time){
					closest.set(mCacheCollision1);
				}
				else if(closest.time==mCacheCollision1.time){
					Math.add(closest.normal,mCacheCollision1.normal);
					boolean b=Math.normalizeCarefully(closest.normal,mEpsilon);
					if(b==false){
						closest.set(mCacheCollision1);
					}
				}
			}
		}
	}

	// Depends upon mSpacialCollection being updated, so not public
	protected void traceSolid(HopEntity solid,Vector3 start,Vector3 end,Collision closest){
		closest.time=-Math.ONE;

		if(solid.mInteraction==HopEntity.INTERACTION_NONE){
			return;
		}

		Segment s=mCacheSegment1;s.set(start,end);

		HopEntity solid2;
	
		// Box tracing moved out to the main loop

		int i;
		for(i=0;i<mNumSpacialCollection;++i){
			solid2=mSpacialCollection[i];
			if(solid!=solid2 && solid2.mInteraction==HopEntity.INTERACTION_NORMAL){
				mCacheCollision1.time=-Math.ONE;
				testSolid(solid,solid2,s,mCacheCollision1);
				if(mCacheCollision1.time>=0){
					if(closest.time<0 || mCacheCollision1.time<closest.time){
						closest.set(mCacheCollision1);
					}
					else if(closest.time==mCacheCollision1.time){
						Math.add(closest.normal,mCacheCollision1.normal);
						boolean b=Math.normalizeCarefully(closest.normal,mEpsilon);
						if(b==false){
							closest.set(mCacheCollision1);
						}
					}
				}

			}
		}

		if(mManager!=null){
			mCacheCollision1.time=-Math.ONE;
			mManager.traceSolid(s,solid,mCacheCollision1);
			if(mCacheCollision1.time>=0){
				if(closest.time<0 || mCacheCollision1.time<closest.time){
					closest.set(mCacheCollision1);
				}
				else if(closest.time==mCacheCollision1.time){
					Math.add(closest.normal,mCacheCollision1.normal);
					boolean b=Math.normalizeCarefully(closest.normal,mEpsilon);
					if(b==false){
						closest.set(mCacheCollision1);
					}
				}
			}
		}
	}

	protected void testSolid(HopEntity solid1,HopEntity solid2,Segment segment,Collision closest){
		mCacheCollision2.reset();
		mCacheCollision2.solid=solid2;

		Shape[] shapes1=solid1.mShapes;
		int numShapes1=solid1.mNumShapes;
		Shape[] shapes2=solid2.mShapes;
		int numShapes2=solid2.mNumShapes;
		Shape shape1;
		Shape shape2;

		int i,j;
		for(i=0;i<numShapes1;++i){
			shape1=shapes1[i];

			for(j=0;j<numShapes2;++j){
				shape2=shapes2[j];

				// No need to reset mCacheCollision2 or it's time here since the trace functions are gaurenteed to do that

				// AABox collisions
				if(shape1.mType==Shape.TYPE_AABOX && shape2.mType==Shape.TYPE_AABOX){
					// Create one big box and then do our tests with a box and a point
					AABox box1=shape1.mAABox;
					AABox box2=shape2.mAABox;

					AABox box=mCacheBox4;box.set(box2);
					Math.add(box,solid2.mPosition);
					Math.sub(box.maxs,box1.mins);
					Math.sub(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_AABOX && shape2.mType==Shape.TYPE_SPHERE){
					// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=shape1.mAABox;
					AABox box=mCacheBox4;box.set(shape2.mSphere.radius);
					Math.add(box,shape2.mSphere.origin);

					Math.add(box,solid2.mPosition);
					Math.sub(box.maxs,box1.mins);
					Math.sub(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_AABOX && shape2.mType==Shape.TYPE_CAPSULE){
					// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=shape1.mAABox;
					AABox box=mCacheBox4;
					shape2.getBound(box);

					Math.add(box,solid2.mPosition);
					Math.sub(box.maxs,box1.mins);
					Math.sub(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				// Sphere collisions
				else if(shape1.mType==Shape.TYPE_SPHERE && shape2.mType==Shape.TYPE_AABOX){
					// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=mCacheBox4;box1.set(shape1.mSphere.radius);
					Math.add(box1,shape1.mSphere.origin);

					AABox box=mCacheBox5;box.set(shape2.mAABox);
					Math.add(box,solid2.mPosition);
					Math.sub(box.maxs,box1.mins);
					Math.sub(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_SPHERE && shape2.mType==Shape.TYPE_SPHERE){
					// Construct super sphere
					Vector3 origin=mCacheVector1;origin.set(solid2.mPosition);

					Math.sub(origin,shape1.mSphere.origin);
					Math.add(origin,shape2.mSphere.origin);

					Sphere sphere=mCacheSphere1;sphere.set(origin,shape2.mSphere.radius+shape1.mSphere.radius);

					traceSphere(segment,sphere,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_SPHERE && shape2.mType==Shape.TYPE_CAPSULE){
					// Construct super capsule
					Vector3 origin=mCacheVector1;origin.set(solid2.mPosition);

					Math.sub(origin,shape1.mSphere.origin);
					Math.add(origin,shape2.mCapsule.origin);

					Capsule capsule=mCacheCapsule1;capsule.set(origin,shape2.mCapsule.direction,shape2.mCapsule.radius+shape1.mSphere.radius);

					traceCapsule(segment,capsule,mCacheCollision2);
				}
				// Capsule collisions
				else if(shape1.mType==Shape.TYPE_CAPSULE && shape2.mType==Shape.TYPE_AABOX){
					// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=mCacheBox4;
					shape1.getBound(box1);
					AABox box=mCacheBox5;box.set(shape2.mAABox);

					Math.add(box,solid2.mPosition);
					Math.sub(box.maxs,box1.mins);
					Math.sub(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_CAPSULE && shape2.mType==Shape.TYPE_SPHERE){
					// Construct super capsule
					Vector3 origin=mCacheVector1;origin.set(solid2.mPosition);
					Math.sub(origin,shape1.mCapsule.origin);
					Math.add(origin,shape2.mSphere.origin);

					// You must invert the capsule direction when forming the supercapsule at the sphere's location
					Vector3 direction=mCacheDirection;direction.set(shape1.mCapsule.direction);
					Math.neg(direction);
					Capsule capsule=mCacheCapsule1;capsule.set(origin,direction,shape1.mCapsule.radius+shape2.mSphere.radius);

					traceCapsule(segment,capsule,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_CAPSULE && shape2.mType==Shape.TYPE_CAPSULE){
//#if false
					// The origin of the 2nd capsule wrt the 1st shape's position
					Vector3 origin=(solid2.mPosition)-(shape1.mSphere.origin)+(shape2.mSphere.origin);
					
					// Find the closest point on the 1st capsule to the 2nd's origin
					Segment s1;
					s1.origin=solid1.mPosition;
					s1.direction=shape1.mCapsule.direction;
					Vector3 o;
					getClosestPointOnSegment(o,s1,origin);
					
					// Construct super capsule
					Capsule capsule(origin+o,shape2.mCapsule.direction,shape1.mCapsule.radius+shape2.mCapsule.radius);
					traceCapsule(segment,capsule,mCacheCollision1);
//#endif
					// Construct super capsule
					Vector3 origin=mCacheVector1;origin.set(solid2.mPosition);
					Math.sub(origin,shape1.mCapsule.origin);
					Math.add(origin,shape2.mCapsule.origin);

					Capsule capsule=mCacheCapsule1;capsule.set(origin,shape1.mCapsule.direction,shape1.mCapsule.radius+shape2.mCapsule.radius);

					traceCapsule(segment,capsule,mCacheCollision2);
				}

				if(mCacheCollision2.time>=0){
					if(closest.time<0 || mCacheCollision2.time<closest.time){
						closest.set(mCacheCollision2);
					}
					else if(closest.time==mCacheCollision2.time){
						Math.add(closest.normal,mCacheCollision2.normal);
						boolean b=Math.normalizeCarefully(closest.normal,mEpsilon);
						if(b==false){
							closest.set(mCacheCollision2);
						}
					}
				}
			}
		}
	}
	
	protected void testLine(HopEntity solid,Segment segment,Collision closest){
		mCacheCollision2.reset();
		mCacheCollision2.solid=solid;

		Shape[] shapes=solid.mShapes;
		int numShapes=solid.mNumShapes;

		Shape shape;

		int i;
		for(i=0;i<numShapes;++i){
			// No need to reset mCacheCollision2 or it's time here since the trace functions are gaurenteed to do that

			shape=shapes[i];
			switch(shape.mType){
				case(Shape.TYPE_AABOX):
					{
						AABox box=mCacheBox4;box.set(shape.mAABox);

						Math.add(box,solid.mPosition);
						traceAABox(segment,box,mCacheCollision2);
					}
				break;
				case(Shape.TYPE_SPHERE):
					{
						Sphere sphere=mCacheSphere1;sphere.set(shape.mSphere);

						Math.add(sphere,solid.mPosition);
						traceSphere(segment,sphere,mCacheCollision2);
					}
				break;
				case(Shape.TYPE_CAPSULE):
					{
						Capsule capsule=mCacheCapsule1;capsule.set(shape.mCapsule);

						Math.add(capsule,solid.mPosition);
						traceCapsule(segment,capsule,mCacheCollision2);
					}
				break;
			}

			if(mCacheCollision2.time>=0){
				if(closest.time<0 || mCacheCollision2.time<closest.time){
					closest.set(mCacheCollision2);
				}
				else if(closest.time==mCacheCollision2.time){
					Math.add(closest.normal,mCacheCollision2.normal);
					boolean b=Math.normalizeCarefully(closest.normal,mEpsilon);
					if(b==false){
						closest.set(mCacheCollision2);
					}
				}
			}
		}
	}

	protected void traceAABox(Segment segment,AABox box,Collision c){
		// We offset boxes to ensure that they stay in the 'half epsilon' borders
		// This lets us not worry about shifting box planes to detect the closest outside,
		// like we have to do with the sphere

		// Use QuarterEpsilon instead of Half, since a quarter will ensure where we get rounded to
		//AABox b=new AABox(box);
		AABox b=mCacheBox6;
		b.set(box);

		// I dont think this is necessary anymore
		//b.maxs.x-=mQuarterEpsilon;
		//b.maxs.y-=mQuarterEpsilon;
		//b.maxs.z-=mQuarterEpsilon;
		//b.mins.x+=mQuarterEpsilon;
		//b.mins.y+=mQuarterEpsilon;
		//b.mins.z+=mQuarterEpsilon;

		if(Math.testInside(segment.origin,b)){
			int dix=java.lang.Math.abs(segment.origin.x-b.mins.x);
			int diy=java.lang.Math.abs(segment.origin.y-b.mins.y);
			int diz=java.lang.Math.abs(segment.origin.z-b.mins.z);
			int dax=java.lang.Math.abs(segment.origin.x-b.maxs.x);
			int day=java.lang.Math.abs(segment.origin.y-b.maxs.y);
			int daz=java.lang.Math.abs(segment.origin.z-b.maxs.z);

			if(dix<=diy && dix<=diz && dix<=dax && dix<=day && dix<=daz){
				if(Math.dot(segment.direction,Math.NEG_X_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Math.NEG_X_UNIT_VECTOR3);
			}
			else if(diy<=diz && diy<=dax && diy<=day && diy<=daz){
				if(Math.dot(segment.direction,Math.NEG_Y_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Math.NEG_Y_UNIT_VECTOR3);
			}
			else if(diz<=dax && diz<=day && diz<=daz){
				if(Math.dot(segment.direction,Math.NEG_Z_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Math.NEG_Z_UNIT_VECTOR3);
			}
			else if(dax<=day && dax<=daz){
				if(Math.dot(segment.direction,Math.X_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Math.X_UNIT_VECTOR3);
			}
			else if(day<=daz){
				if(Math.dot(segment.direction,Math.Y_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Math.Y_UNIT_VECTOR3);
			}
			else{
				if(Math.dot(segment.direction,Math.Z_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Math.Z_UNIT_VECTOR3);
			}

			c.time=0;
			c.point.set(segment.origin);
		}
		else{
			c.time=Math.findIntersection(segment,b,c.point,c.normal);
		}
	}

	protected void traceSphere(Segment segment,Sphere sphere,Collision c){
		// Check to see if we're inside
		if(Math.testInside(segment.origin,sphere)){
			// Skip the 'closer in' checks on spheres, since now matter what we do,
			// Apparently we still slide in closer to spheres, very slightly.
			// I'll really need to look at this more some time
			// So we always just push anyone in a sphere out more
			{
				// If we're trying to trace farther into the sphere
				Vector3 n=mCacheVector4;n.set(segment.origin);
				
				Math.sub(n,sphere.origin);
				Math.normalizeCarefully(n,mEpsilon);
				// Maybe this calculation is inacurate to test if we might get 'epsilon bumped' in?
				if(Math.dot(n,segment.direction)<=mEpsilon){
					c.time=0;
					c.point.set(segment.origin);
					Math.normalizeCarefully(c.normal,n,mEpsilon);
				}
				else{
					c.time=-Math.ONE;
				}
			}
		}
		else{
			c.time=Math.findIntersection(segment,sphere,c.point,c.normal);
		}
	}

	protected void traceCapsule(Segment segment,Capsule capsule,Collision c){
		Vector3 p1=mCacheVector2;
		Vector3 p2=mCacheVector3;
		Segment s=mCacheSegment2;

		// Capsules are really just segments, so test against a sphere located at the closest point of the capsule segment
		s.origin.set(capsule.origin);
		s.direction.set(capsule.direction);
		Math.getClosestPointsOnSegments(p1,p2,s,segment,mEpsilon);

		Sphere sphere=mCacheSphere1;sphere.set(p1,capsule.radius);
		traceSphere(segment,sphere,c);
	}

	protected void frictionLink(HopEntity solid,Vector3 solidVelocity,HopEntity hitSolid,int fdt,Vector3 result){
		result.set(Math.ZERO_VECTOR3);

		// Andrew's friction linking code
		if(hitSolid!=null && solid.mMass>0 && hitSolid.mMass!=0 && (solid.mCoefficientOfStaticFriction>0 || solid.mCoefficientOfDynamicFriction>0)){
			int fn=0,lenVr=0;
			//Vector3 vr=new Vector3();
			Vector3 vr=mCachevr;
			vr.set(Math.ZERO_VECTOR3);
			//Vector3 ff=new Vector3();
			Vector3 ff=mCacheff;
			ff.set(Math.ZERO_VECTOR3);
			//Vector3 fs=new Vector3();
			Vector3 fs=mCachefs;
			fs.set(Math.ZERO_VECTOR3);
			//Vector3 normVr=new Vector3();
			Vector3 normVr=mCachenormVr;
			normVr.set(Math.ZERO_VECTOR3);

			fn=
				//#exec fpcomp.bat "Math.dot(mGravity,solid.mTouchingNormal)*solid.mCoefficientOfGravity*solid.mMass+Math.dot(solid.mForce,solid.mTouchingNormal)"
				;
			Math.sub(vr,solidVelocity,hitSolid.mVelocity);
			lenVr=Math.length(vr);
			Math.div(normVr,vr,lenVr);

			if(fn!=0 && lenVr>0){
				Math.mul(ff,normVr,fn);
				Math.mul(result,ff,solid.mCoefficientOfStaticFriction);

				// Multiply fdt in here to avoid exessively large numbers when calculating fs
				Math.mul(result,fdt);
			
				// This is the static force required to resist relative velocity and any applied forces in this time step
				Math.mul(fs,vr,-solid.mMass);
				Math.mul(normVr,Math.dot(solid.mForce,normVr));
				Math.mul(normVr,fdt);
				Math.add(fs,normVr);

				if(Math.lengthSquared(fs)>Math.lengthSquared(result)){
					Math.mul(result,ff,solid.mCoefficientOfDynamicFriction);
				}
				else{
					Math.div(result,fs,fdt);
				}
			}
		}
	}

	byte mIntegratorType;
	
	Vector3 mGravity=new Vector3();
	Vector3 mFluidVelocity=new Vector3();

	int mEpsilonBits;
	int mEpsilon;
	int mHalfEpsilon;
	int mQuarterEpsilon;

	int mMaxVelocityComponent;
	Collision[] mCollisions=new Collision[0];
	HopEntity[] mSolids=new HopEntity[0];
	int mNumSolids=0;
	HopEntity[] mSpacialCollection=new HopEntity[0];
	int mNumSpacialCollection=0;

	int mDeactivateSpeed;
	int mDeactivateCount;

	Manager mManager;

	Vector3 mCacheOldPosition=new Vector3();
	Vector3 mCacheNewPosition=new Vector3();
	Vector3 mCacheOffset=new Vector3();
	Vector3 mCacheVelocity=new Vector3();
	Vector3 mCacheTemp=new Vector3();
	Vector3 mCacheT=new Vector3();
	Vector3 mCacheLeftOver=new Vector3();
	Vector3 mCacheFrictionForce=new Vector3();
	Vector3 mCacheK1=new Vector3();
	Vector3 mCacheK2=new Vector3();
	Vector3 mCacheK3=new Vector3();
	Vector3 mCacheK4=new Vector3();
	Collision mCacheUpdateCollision=new Collision();

	Collision mCacheCollision1=new Collision();
	Collision mCacheCollision2=new Collision();

	Vector3 mCacheVector1=new Vector3();
	Vector3 mCacheVector2=new Vector3();
	Vector3 mCacheVector3=new Vector3();
	Vector3 mCacheVector4=new Vector3();
	Vector3 mCacheDirection=new Vector3();
	Segment mCacheSegment1=new Segment();
	Segment mCacheSegment2=new Segment();
	AABox mCacheBox1=new AABox();
	AABox mCacheBox2=new AABox();
	AABox mCacheBox3=new AABox();
	AABox mCacheBox4=new AABox();
	AABox mCacheBox5=new AABox();
	AABox mCacheBox6=new AABox();

	Sphere mCacheSphere1=new Sphere();
	Capsule mCacheCapsule1=new Capsule();
	
	Vector3 mCachevr=new Vector3();
	Vector3 mCacheff=new Vector3();
	Vector3 mCachefs=new Vector3();
	Vector3 mCachenormVr=new Vector3();
}

