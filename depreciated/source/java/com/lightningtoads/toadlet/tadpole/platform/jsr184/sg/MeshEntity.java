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
import javax.microedition.m3g.*;
import java.util.*;

public class MeshEntity extends RenderableEntity{
	public MeshEntity(Engine engine){
		super(engine);
		mType|=TYPE_MESH;

		mVisible=true;

		mLayer=1;
	}
	
	public void destroy(){
		clean();
		
		super.destroy();
	}

	public void load(String name){
		load(name,false);
	}

	// To make this more consistant with the Micro3D implementation, by default only one animated MeshEntity of a type is available
	// Unless you specifically call this load function passing true for multipleAnimatedInstances
	// This also works better with a bug in the Nokia 2E M3G implementation that makes M3G have the same limitation as Micro3D
	public void load(String name,boolean multipleAnimatedInstances){
		clean();

		mMesh=mEngine.getMesh(name);

		if(mMesh==null){
			throw new RuntimeException("Mesh "+name+" not cached");
		}

		if(mMesh instanceof SkinnedMesh){
			if(multipleAnimatedInstances){
				mSkinnedMesh=cloneSkinnedMesh((SkinnedMesh)mMesh,name);
				mMesh=mSkinnedMesh;
			}
			else{
				if(mMesh.getParent()!=null){
					throw new RuntimeException("Using multiple animated instances of mesh, but multipleAnimatedInstances is not set");
				}
				mSkinnedMesh=(SkinnedMesh)mMesh;
			}
			Hashtable hashtable=(Hashtable)mMesh.getUserObject();
			if(hashtable!=null){
				mControllers=new AnimationController[hashtable.size()];

				Enumeration e=null;
				for(e=hashtable.elements();e.hasMoreElements();){
					byte[] bytes=(byte[])e.nextElement();
					int id=bytes[0];

					AnimationController controller=(AnimationController)mSkinnedMesh.find(id);

					controller.setWeight(0);
					mControllers[id-1]=controller;
				}
			}
		}
		else{
			mMesh=cloneMesh(mMesh,name);
		}

		mAppearances=new Appearance[mMesh.getSubmeshCount()];
		mAppearanceHasTexture=new boolean[mMesh.getSubmeshCount()];

		int i;
		for(i=0;i<mAppearances.length;++i){
			Appearance appearance=mMesh.getAppearance(i);

			Hashtable hashtable=(Hashtable)appearance.getUserObject();
			if(hashtable!=null){
				byte[] bytes=(byte[])hashtable.elements().nextElement();
				if(bytes!=null && bytes.length>0){
					mAppearanceHasTexture[i]=bytes[0]>0;
				}
			}

			if((mMesh instanceof SkinnedMesh)==false || multipleAnimatedInstances){
				mAppearances[i]=cloneAppearance(appearance);
				mAppearances[i].setLayer(mLayer);
				mMesh.setAppearance(i,mAppearances[i]);
			}
			else{
				mAppearances[i]=appearance;
				mAppearances[i].setLayer(mLayer);
			}
		}

		mNode=mMesh;
		
		if(mParent!=null){
			mParent.mGroup.addChild(mNode);
		}
	}

	// Apparently Nokia 2E phones cannot reliable call duplicate on Meshes, so we rebuild it manually
	Mesh cloneMesh(Mesh mesh,String name){
		VertexBuffer vertexBuffer=mesh.getVertexBuffer();

		IndexBuffer[] indexBuffers=new IndexBuffer[mesh.getSubmeshCount()];
		Appearance[] appearances=new Appearance[mesh.getSubmeshCount()];

		int i;
		for(i=0;i<mesh.getSubmeshCount();++i){
			indexBuffers[i]=mesh.getIndexBuffer(i);
			appearances[i]=mesh.getAppearance(i);
		}

		return new Mesh(vertexBuffer,indexBuffers,appearances);
	}

	// Nokia 2E phones cannot successfully duplicate SkinnedMeshes, so we reload them and change our resource pointers
	SkinnedMesh cloneSkinnedMesh(SkinnedMesh skinnedMesh,String name){
		Object3D[] sg=null;
		try{
			sg=Loader.load(name+".m3g");
		}
		catch(java.io.IOException ex){
			throw new RuntimeException(ex.toString());
		}
		SkinnedMesh newSkinnedMesh=(SkinnedMesh)sg[0];

		int i;
		for(i=0;i<skinnedMesh.getSubmeshCount();++i){
			newSkinnedMesh.setAppearance(i,skinnedMesh.getAppearance(i));
		}

		VertexBuffer vertexBuffer=skinnedMesh.getVertexBuffer();
		VertexBuffer newVertexBuffer=newSkinnedMesh.getVertexBuffer();

		VertexArray colors=vertexBuffer.getColors();
		if(colors!=null){
			newVertexBuffer.setColors(colors);
		}

		VertexArray normals=vertexBuffer.getNormals();
		if(normals!=null){
			newVertexBuffer.setNormals(normals);
		}

		float[] positionScaleBias=new float[4];
		VertexArray positions=vertexBuffer.getPositions(positionScaleBias);
		if(positions!=null){
			float[] positionBias=new float[3];
			positionBias[0]=positionScaleBias[1];
			positionBias[1]=positionScaleBias[2];
			positionBias[2]=positionScaleBias[3];
			newVertexBuffer.setPositions(positions,positionScaleBias[0],positionBias);
		}

		// Only copy over the first texCoordSet, probably all we'll use
		float[] texCoordScaleBias=new float[4];
		VertexArray texCoords=vertexBuffer.getTexCoords(0,texCoordScaleBias);
		if(positions!=null){
			float[] texCoordBias=new float[3];
			texCoordBias[0]=texCoordScaleBias[1];
			texCoordBias[1]=texCoordScaleBias[2];
			texCoordBias[2]=texCoordScaleBias[3];
			newVertexBuffer.setTexCoords(0,texCoords,texCoordScaleBias[0],texCoordBias);
		}

		return newSkinnedMesh;
	}

	Appearance cloneAppearance(Appearance appearance){
		Appearance newAppearance=new Appearance();

		newAppearance.setCompositingMode(appearance.getCompositingMode());
		newAppearance.setFog(appearance.getFog());
		newAppearance.setLayer(appearance.getLayer());
		newAppearance.setMaterial(appearance.getMaterial());
		newAppearance.setPolygonMode(appearance.getPolygonMode());
		newAppearance.setTexture(0,appearance.getTexture(0));

		return newAppearance;
	}

	void clean(){
		stopAnimation();

		if(mNode!=null && mNode.getParent()!=null){
			((Group)(mNode.getParent())).removeChild(mNode);
		}
		mNode=null;
		mMesh=null;
		mSkinnedMesh=null;
		mControllers=null;
	}

	public void setTexture(String name){
		if(mMesh==null){
			return;
		}
		
		if(name!=null){
			Texture2D texture=mEngine.getTexture(name);
			int i;
			for(i=0;i<mAppearances.length;++i){
				if(mAppearanceHasTexture[i]){
					mAppearances[i].setTexture(0,texture);
				}
			}
		}
		else{
			int i;
			for(i=0;i<mAppearances.length;++i){
				if(mAppearanceHasTexture[i]){
					mAppearances[i].setTexture(0,null);
				}
			}
		}
	}

	public void setTextures(String[] names){
		if(mMesh==null){
			return;
		}
		
		int i;
		int count=0;
		for(i=0;i<mAppearanceHasTexture.length;++i){
			if(mAppearanceHasTexture[i]){
				count++;
			}
		}

		if(names!=null){
			if(names.length!=count){
				throw new RuntimeException("setTextures: names.length!=num textured appearances, "+names.length+"!="+count);
			}

			count=0;
			for(i=0;i<mAppearanceHasTexture.length;++i){
				if(mAppearanceHasTexture[i]){
					Texture2D texture=mEngine.getTexture(names[count]);
					mAppearances[i].setTexture(0,texture);
					count++;
				}
			}
		}
		else{
			count=0;
			for(i=0;i<mAppearanceHasTexture.length;++i){
				if(mAppearanceHasTexture[i]){
					mAppearances[i].setTexture(0,null);
					count++;
				}
			}
		}
	}

	public void setLayer(int layer){
		mLayer=layer;
		
		if(mMesh==null){
			return;
		}
		
		int i;
		for(i=0;i<mAppearances.length;++i){
			mAppearances[i].setLayer(layer);
		}
	}

	public int getLayer(){
		return mLayer;
	}

	public void setVisible(boolean visible){
		mVisible=visible;
		
		mNode.setRenderingEnable(visible);
	}

	public boolean getVisible(){
		return mVisible;
	}

	public void startAnimation(int animation,boolean loop){
		startAnimation(animation,loop,Math.ONE,false);
	}

	public void startAnimation(int animation,boolean loop,int timeScale,boolean keepLastFrame){
		if(mControllers!=null && animation<mControllers.length){
			if(mActiveController!=-1){
				mControllers[mActiveController].setWeight(0.0f);
				mActiveController=-1;
			}
			else{
				mSceneManager=mEngine.getSceneManager();
				mSceneManager.registerAnimatedMeshEntity(this);
			}

			mActiveController=animation;
			mRefWorldTime=mEngine.getSceneManager().getTime();
			mAnimationTimeScale=timeScale;
			mControllers[mActiveController].setPosition(0,mRefWorldTime);
			mControllers[mActiveController].setSpeed(Math.fixedToFloat(timeScale),mRefWorldTime);
			mControllers[mActiveController].setWeight(1.0f);

			mLoopAnimation=loop;
			mKeepLastFrame=keepLastFrame;

			mNode.animate(mEngine.getSceneManager().getTime());
		}
		else{
			throw new RuntimeException("Invalid controller");
		}
	}

	public void stopAnimation(){
		if(mActiveController!=-1){
			mControllers[mActiveController].setWeight(0.0f);
			mActiveController=-1;
			if(mSceneManager!=null){
				mSceneManager.unregisterAnimatedMeshEntity(this);
				mSceneManager=null;
			}
		}
	}

	public Mesh getMesh(){
		return mMesh;
	}

	public SkinnedMesh getSkinnedMesh(){
		return mSkinnedMesh;
	}

	Appearance[] mAppearances;
	boolean[] mAppearanceHasTexture;
	Mesh mMesh;
	int mLayer;
	boolean mVisible;
	SkinnedMesh mSkinnedMesh;
	AnimationController[] mControllers;
	int mActiveController=-1;
	int mRefWorldTime;
	int mAnimationTimeScale;
	boolean mLoopAnimation;
	boolean mKeepLastFrame;

	SceneManager mSceneManager;
}
