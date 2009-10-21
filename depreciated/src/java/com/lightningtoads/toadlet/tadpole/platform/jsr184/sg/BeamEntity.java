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

public class BeamEntity extends RenderableEntity{
	public BeamEntity(Engine engine){
		super(engine);
		mType|=TYPE_BEAM;
		
		PolygonMode mode=new PolygonMode();
		mode.setCulling(PolygonMode.CULL_NONE);
		mAppearance.setPolygonMode(mode);
		
		mAppearance.setLayer(1);
	}
	
	public void destroy(){
		clean();
		
		super.destroy();
	}

	public void setLayer(int layer){
		mAppearance.setLayer(layer);
	}

	public int getLayer(){
		return mAppearance.getLayer();
	}

	public void setVisible(boolean visible){
		mNode.setRenderingEnable(visible);
	}

	public boolean getVisible(){
		return mNode.isRenderingEnabled();
	}

	public void setBlending(boolean blend,boolean add){
		if(mCompositing==null){
			mCompositing=new CompositingMode();
		}
		
		if(blend){
			if(add){
				mCompositing.setBlending(CompositingMode.ALPHA_ADD);
			}
			else{
				mCompositing.setBlending(CompositingMode.ALPHA);
			}
		}
		else{
			mCompositing.setBlending(CompositingMode.REPLACE);
		}
		
		mAppearance.setCompositingMode(mCompositing);
	}

	public void setTransparent(boolean t){
		if(mCompositing==null){
			mCompositing=new CompositingMode();
		}

		if(true){
			mCompositing.setAlphaThreshold(0.5f);
			mCompositing.setDepthWriteEnable(false);
		}
		else{
			mCompositing.setAlphaThreshold(0.0f);
			mCompositing.setDepthWriteEnable(true);
		}
		mAppearance.setCompositingMode(mCompositing);
	}

	public void startBeam(int color,int scale,Vector3 start,Vector3 end){
		clean();

		VertexBuffer vertexes=new VertexBuffer();
		
		// Set up a small triangle strip		
		VertexArray positions=new VertexArray(4,3,1);
		positions.set(0,4,new byte[]{1,0,0,-1,0,0,1,0,1,-1,0,1});
		vertexes.setPositions(positions,1.0f,null);

		vertexes.setDefaultColor(color);

		IndexBuffer indexes=new TriangleStripArray(0,new int[]{4});

		mMesh=new Mesh(vertexes,indexes,mAppearance);
		mNode=mMesh;

		if(mParent!=null){
			mParent.mGroup.addChild(mNode);
		}
		
		mStart.set(start);
		mEnd.set(end);

		mScale=scale;

		updatePoints();
	}

	void clean(){
		if(mNode!=null && mNode.getParent()!=null){
			((Group)(mNode.getParent())).removeChild(mNode);
		}
		mNode=null;
		mMesh=null;
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
		Math.sub(mCacheDir,mEnd,mStart);
		int length=Math.length(mCacheDir);
		if(length==0){
			setVisible(false);
			return;
		}
		
		Math.div(mCacheDir,length);
		if(mCacheDir.equals(Math.Z_UNIT_VECTOR3) || mCacheDir.equals(Math.NEG_Z_UNIT_VECTOR3)){
			setVisible(false);
			return;
		}
		
		setVisible(true);

		Math.cross(mCacheRight,mCacheDir,Math.Z_UNIT_VECTOR3);
		Math.normalizeCarefully(mCacheRight,0);
		Math.cross(mCacheUp,mCacheRight,mCacheDir);

		if(mTransform==null){
			mTransform=new Transform();
			mTransformData=new float[16];
			mTransformData[15]=1.0f;
		}

		mTransformData[0]=Math.fixedToFloat(mCacheRight.x);
		mTransformData[4]=Math.fixedToFloat(mCacheRight.y);
		mTransformData[8]=Math.fixedToFloat(mCacheRight.z);
		mTransformData[1]=Math.fixedToFloat(mCacheUp.x);
		mTransformData[5]=Math.fixedToFloat(mCacheUp.y);
		mTransformData[9]=Math.fixedToFloat(mCacheUp.z);
		mTransformData[2]=Math.fixedToFloat(mCacheDir.x);
		mTransformData[6]=Math.fixedToFloat(mCacheDir.y);
		mTransformData[10]=Math.fixedToFloat(mCacheDir.z);

		mTransformData[3]=Math.fixedToFloat(mStart.x);
		mTransformData[7]=Math.fixedToFloat(mStart.y);
		mTransformData[11]=Math.fixedToFloat(mStart.z);
		
		mTransform.set(mTransformData);
		
		mTransform.postScale(Math.fixedToFloat(mScale),1,Math.fixedToFloat(length));
		
		mNode.setTransform(mTransform);
	}

	Mesh mMesh;
	Appearance mAppearance=new Appearance();
	CompositingMode mCompositing;
	int mScale=Math.ONE;
	Vector3 mStart=new Vector3();
	Vector3 mEnd=new Vector3();
	Vector3 mCacheDir=new Vector3();
	Vector3 mCacheUp=new Vector3();
	Vector3 mCacheRight=new Vector3();
}
