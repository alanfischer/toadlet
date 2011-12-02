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

public class BeamEntity extends RenderableEntity{
	public BeamEntity(Engine engine){
		super(engine);
		mType|=TYPE_BEAM;

		VertexFormat vertexFormat=new VertexFormat(2);
		vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_POSITION,VertexElement.FORMAT_BIT_FIXED_32|VertexElement.FORMAT_BIT_COUNT_3));
		vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_COLOR,VertexElement.FORMAT_COLOR_RGBA));
		Buffer vertexBuffer=new Buffer(Buffer.UT_STATIC,Buffer.AT_WRITE_ONLY,vertexFormat,4);
		mVertexData=new VertexData(vertexBuffer);

		mIndexData=new IndexData(IndexData.PRIMITIVE_TRISTRIP,null,0,4);

		mMaterial=new Material();
		mMaterial.setFaceCulling(Material.FC_NONE);
	}

	public void destroy(){
		super.destroy();
	}

	public void startBeam(int color,int scale,Vector3 start,Vector3 end){
		mColor=(color&0xFF000000) | ((color>>16)&0xFF) | (color&0xFF00) | ((color<<16)&0xFF0000);

		{
			java.nio.IntBuffer data=(java.nio.IntBuffer)mVertexData.getVertexBuffer(0).lock(Buffer.LT_WRITE_ONLY);

			// Write all the data every time, since OGL hardware buffers dont seem to remember previous data correctly
			data.put(0,Math.ONE);
			data.put(1,0);
			data.put(2,0);

			data.put(3,mColor);

			data.put(4,-Math.ONE);
			data.put(5,0);
			data.put(6,0);

			data.put(7,mColor);

			data.put(8,Math.ONE);
			data.put(9,0);
			data.put(10,Math.ONE);

			data.put(11,mColor);

			data.put(12,-Math.ONE);
			data.put(13,0);
			data.put(14,Math.ONE);

			data.put(15,mColor);

			mVertexData.getVertexBuffer(0).unlock();
		}

		mStart.set(start);
		mEnd.set(end);

		mScale=scale;

		updatePoints();
	}

	public void setStartPoint(Vector3 start){
		mStart.set(start);
		updatePoints();
	}

	public void setEndPoint(Vector3 end){
		mEnd.set(end);
		updatePoints();
	}

	public Material getMaterial(){return mMaterial;}

	void updatePoints(){
		Math.sub(mCacheDir,mEnd,mStart);
		int length=Math.length(mCacheDir);
		if(length==0){
			setVisible(false);
			return;
		}

		setVisible(true);

		Math.div(mCacheDir,length);
		Math.cross(mCacheRight,mCacheDir,Math.Z_UNIT_VECTOR3);
		if(Math.normalizeCarefully(mCacheRight,0)==false){
			Math.cross(mCacheRight,mCacheDir,Math.X_UNIT_VECTOR3);
			Math.normalizeCarefully(mCacheRight,0);
		}
		Math.cross(mCacheUp,mCacheRight,mCacheDir);

		Matrix4x4 transform=mCacheTransform;

		transform.setAt(0,0,mCacheRight.x);
		transform.setAt(1,0,mCacheRight.y);
		transform.setAt(2,0,mCacheRight.z);
		transform.setAt(0,1,mCacheUp.x);
		transform.setAt(1,1,mCacheUp.y);
		transform.setAt(2,1,mCacheUp.z);
		transform.setAt(0,2,mCacheDir.x);
		transform.setAt(1,2,mCacheDir.y);
		transform.setAt(2,2,mCacheDir.z);

		Matrix4x4 scale=mCacheScale;
		scale.setAt(0,0,mScale);
		scale.setAt(1,1,Math.ONE);
		scale.setAt(2,2,length);

		Math.mul(mTransform,transform,scale);

		mTransform.setAt(0,3,mStart.x);
		mTransform.setAt(1,3,mStart.y);
		mTransform.setAt(2,3,mStart.z);
	}

	int mColor=0xFFFFFFFF;
	int mScale=Math.ONE;
	Vector3 mStart=new Vector3();
 	Vector3 mEnd=new Vector3();
 	Vector3 mOffset=new Vector3();

	Material mMaterial;
	VertexData mVertexData;
	IndexData mIndexData;

	// Warning: Not multiple engine in multiple thread safe
	static Vector3 mCacheRight=new Vector3();
	static Vector3 mCacheUp=new Vector3();
	static Vector3 mCacheDir=new Vector3();
	static Matrix4x4 mCacheTransform=new Matrix4x4();
	static Matrix4x4 mCacheScale=new Matrix4x4();

	SceneManager mSceneManager;
}
