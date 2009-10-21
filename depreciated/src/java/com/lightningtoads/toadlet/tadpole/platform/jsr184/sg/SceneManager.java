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

import javax.microedition.lcdui.*;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import javax.microedition.m3g.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

public class SceneManager extends NodeEntity{
	public SceneManager(Engine engine){
		super(engine);

		mWorld=new World();
		mGroup=mWorld;
		mNode=mGroup;

		Background background=new Background();
		background.setDepthClearEnable(true);
		background.setColorClearEnable(false);
		mWorld.setBackground(background);

		mBackgroundNode=new NodeEntity(engine);
		attach(mBackgroundNode);
	}

	public final NodeEntity getBackgroundNode(){
		return mBackgroundNode;
	}

	public void setTime(int time){
		mTime=time;
	}

	public int getTime(){
		return mTime;
	}

	public final void setAmbientColor(int ambient){
		if(ambient>0){
			if(mAmbientLight==null){
				mAmbientLight=new Light();
				mAmbientLight.setMode(Light.AMBIENT);
				mWorld.addChild(mAmbientLight);
			}
			mAmbientLight.setColor(ambient);
		}
		else if(mAmbientLight!=null){
			mWorld.removeChild(mAmbientLight);
			mAmbientLight=null;
		}
	}

	public final int getAmbientColor(){
		if(mAmbientLight!=null){
			return mAmbientLight.getColor();
		}
		else{
			return 0;
		}
	}

	public void update(int dt){
		//#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"update called on a destroyed SceneManager");
				throw new RuntimeException("update called on a destroyed SceneManager");
			}
		//#endif

		mTime+=dt;

		// Update sprites
		int i;
		for(i=0;i<mNumSpriteEntities;++i){
			SpriteEntity sprite=mSpriteEntities[i];
			if(sprite.mFrameTime!=0){
				sprite.mTime+=dt;
				if(sprite.mTime>sprite.mFrameTime){
					sprite.mTime-=sprite.mFrameTime;
					if(sprite.mTargetFrame==-1){
						sprite.mFrame++;
						if(sprite.mFrame>=sprite.mSprite.numFrames){
							if(sprite.mDieOnFinish){
								sprite.destroy();
								continue;
							}
							sprite.mFrame=0;
						}
					}
					else{
						if(sprite.mTargetFrame<sprite.mFrame){
							sprite.mFrame--;
						}
						else if(sprite.mTargetFrame>sprite.mFrame){
							sprite.mFrame++;
						}
						else if(sprite.mDieOnFinish){
							sprite.destroy();
							continue;
						}
					}

					sprite.updateFrame();
				}
			}
		}

		for(i=0;i<mNumAnimatedMeshEntities;++i){
			MeshEntity mesh=mAnimatedMeshEntities[i];
			if(mesh.mActiveController!=-1){
				AnimationController controller=mesh.mControllers[mesh.mActiveController];
				// Here we are using the fact that the activeInterval start & end are the same & compiled into the mesh
				// We use them to represent the length of the animation
				if((Math.fixedToInt(Math.mul(Math.intToFixed(mTime-mesh.mRefWorldTime),mesh.mAnimationTimeScale))+mesh.mRefWorldTime)>=(mesh.mRefWorldTime+controller.getActiveIntervalStart())){
					if(mesh.mLoopAnimation==true){
						controller.setPosition(0,mTime);
						mesh.mRefWorldTime=mTime;
					}
					else if(mesh.mKeepLastFrame==false){
						mesh.stopAnimation();
					}
				}
			}
		}

		// Animate the scene
		mWorld.animate(mTime);
	}

	// Not final, so it can be overridden in the HopSceneManager
	public void render(Graphics g,CameraEntity camera){
		//#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"render called on a destroyed SceneManager");
				throw new RuntimeException("render called on a destroyed SceneManager");
			}
		//#endif

		//#ifdef TOADLET_DEBUG
			Profile.beginSection(Profile.SECTION_RENDER);
		//#endif

		Graphics3D graphics3D=mEngine.getGraphics3D();

		int clipX=g.getClipX();
		int clipY=g.getClipY();
		int clipWidth=g.getClipWidth();
		int clipHeight=g.getClipHeight();

		int viewportX=clipX;
		int viewportY=clipY;
		int viewportWidth=clipWidth;
		int viewportHeight=clipHeight;

		int translateX=g.getTranslateX();
		int translateY=g.getTranslateY();

		if(camera.mViewportSet){
			viewportX=camera.getViewportX();
			viewportY=camera.getViewportY();
			viewportWidth=camera.getViewportWidth();
			viewportHeight=camera.getViewportHeight();
		}

		// We don't use setViewport, because it seems to have no effect on some devices, but setClip works
		g.setClip(viewportX,viewportY,viewportWidth,viewportHeight);
		g.translate(-translateX,-translateY);

		if(mBackgroundNode.mNumEntities>0 && camera.mTransformData!=null){
			mBackgroundNode.mNode.setTranslation(camera.mTransformData[3],camera.mTransformData[7],camera.mTransformData[11]);
		}

		try{
			graphics3D.bindTarget(g);
			graphics3D.render(mWorld);
		}
		catch(Exception ex){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"SceneManager.render:"+ex.toString());
			//#endif
		}
		finally{
			graphics3D.releaseTarget();
		}

		g.translate(translateX,translateY);
		g.setClip(clipX,clipY,clipWidth,clipHeight);

		//#ifdef TOADLET_DEBUG
			Profile.endSection(Profile.SECTION_RENDER);
		//#endif
	}

	public final World getWorld(){
		return mWorld;
	}

	final void registerSpriteEntity(SpriteEntity entity){
		int i;
		for(i=0;i<mNumSpriteEntities;++i){
			if(mSpriteEntities[i]==entity)break;
		}

		if(i==mNumSpriteEntities){
			if(mSpriteEntities.length<mNumSpriteEntities+1){
				SpriteEntity[] entities=new SpriteEntity[mNumSpriteEntities+1];
				System.arraycopy(mSpriteEntities,0,entities,0,mSpriteEntities.length);
				mSpriteEntities=entities;
			}

			mSpriteEntities[mNumSpriteEntities]=entity;
			mNumSpriteEntities++;
		}
	}

	final void unregisterSpriteEntity(SpriteEntity entity){
		int i;
		for(i=0;i<mNumSpriteEntities;++i){
			if(mSpriteEntities[i]==entity)break;
		}

		if(i!=mNumSpriteEntities){
			mSpriteEntities[i]=null;
			System.arraycopy(mSpriteEntities,i+1,mSpriteEntities,i,mNumSpriteEntities-(i+1));
			mNumSpriteEntities--;
			mSpriteEntities[mNumSpriteEntities]=null;
		}
	}

	final void registerAnimatedMeshEntity(MeshEntity entity){
		int i;
		for(i=0;i<mNumAnimatedMeshEntities;++i){
			if(mAnimatedMeshEntities[i]==entity)break;
		}

		if(i==mNumAnimatedMeshEntities){
			if(mAnimatedMeshEntities.length<mNumAnimatedMeshEntities+1){
				MeshEntity[] entities=new MeshEntity[mNumAnimatedMeshEntities+1];
				System.arraycopy(mAnimatedMeshEntities,0,entities,0,mAnimatedMeshEntities.length);
				mAnimatedMeshEntities=entities;
			}

			mAnimatedMeshEntities[mNumAnimatedMeshEntities]=entity;
			mNumAnimatedMeshEntities++;
		}
	}

	final void unregisterAnimatedMeshEntity(MeshEntity entity){
		int i;
		for(i=0;i<mNumAnimatedMeshEntities;++i){
			if(mAnimatedMeshEntities[i]==entity)break;
		}

		if(i!=mNumAnimatedMeshEntities){
			mAnimatedMeshEntities[i]=null;
			System.arraycopy(mAnimatedMeshEntities,i+1,mAnimatedMeshEntities,i,mNumAnimatedMeshEntities-(i+1));
			mNumAnimatedMeshEntities--;
			mAnimatedMeshEntities[mNumAnimatedMeshEntities]=null;
		}
	}

	int mTime=0;

	NodeEntity mBackgroundNode;

	World mWorld;

	SpriteEntity[] mSpriteEntities=new SpriteEntity[0];
	int mNumSpriteEntities;

	MeshEntity[] mAnimatedMeshEntities=new MeshEntity[0];
	int mNumAnimatedMeshEntities;

	Light mAmbientLight;
}
