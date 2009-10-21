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

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;

public class MeshEntity extends RenderableEntity{
	public final class SubMesh{
		public SubMesh(MeshEntity meshEntity){
			this.meshEntity=meshEntity;
		}

		public Material material;
		public IndexData indexData;
		public VertexData vertexData;
		public MeshEntity meshEntity;
	}

	public final static class Bone{
		public Bone(){}

		public int boneIndex;

		public Bone parent;
		public Bone[] children;

		public Vector3 localTranslate=new Vector3();
		public Quaternion localRotate=new Quaternion();

		public Vector3 worldTranslate=new Vector3();
		public Quaternion worldRotate=new Quaternion();
		public Matrix3x3 worldRotateMatrix=new Matrix3x3();
		
		public Vector3 boneSpaceTranslate=new Vector3();
		public Matrix3x3 boneSpaceRotate=new Matrix3x3();
	}

	public MeshEntity(Engine engine){
		super(engine);
		mType|=TYPE_MESH;

		mCurrentAnimation=-1;
		mAnimationTimeScale=Math.ONE;
	}
	
	public void destroy(){
		stopAnimation();

		super.destroy();
	}

	public void load(String name){
		load(mEngine.getMesh(name));
	}

	public void load(Mesh mesh){
		mMesh=mesh;

		if(mMesh==null){
			throw new RuntimeException("Invalid Mesh");
		}

		if(mesh.boundingRadius!=-Math.ONE){
			mBoundingRadius=Math.mul(mesh.worldScale,mesh.boundingRadius);
		}
		else{
			mBoundingRadius=-Math.ONE;
		}

		int i;
		if(mMesh.bones!=null && mMesh.bones.length>0){
			mBones=new Bone[mMesh.bones.length];
			for(i=0;i<mMesh.bones.length;++i){
				Bone bone=new Bone();
				bone.boneIndex=i;
				mBones[i]=bone;
			}

			for(i=0;i<mBones.length;++i){
				Bone bone=mBones[i];

				byte parent=mMesh.bones[bone.boneIndex].parent;
				if(parent!=(byte)-1){
					bone.parent=mBones[parent];
				}
				else{
					bone.parent=null;
				}

				if(mMesh.bones[bone.boneIndex].children!=null){
					bone.children=new Bone[mMesh.bones[bone.boneIndex].children.length];
					int j;
					for(j=0;j<mMesh.bones[bone.boneIndex].children.length;++j){
						bone.children[j]=mBones[mMesh.bones[bone.boneIndex].children[j]];
					}
				}
			}

			mTrackHints=new int[mBones.length];

			createVertexBuffer();

			updateAnimation(0);
		}

		mSubMeshes=new SubMesh[mMesh.subMeshes.length];
		for(i=0;i<mMesh.subMeshes.length;++i){
			SubMesh subMesh=new SubMesh(this);
			mSubMeshes[i]=subMesh;
			subMesh.material=mMesh.subMeshes[i].material;
			subMesh.indexData=mMesh.subMeshes[i].indexData;
			if(mDynamicVertexData!=null){
				subMesh.vertexData=mDynamicVertexData;
			}
			else{
				subMesh.vertexData=mMesh.staticVertexData;
			}
		}
	}

	public SubMesh getSubMesh(String name){
		int i;
		for(i=0;i<mMesh.subMeshes.length;++i){
			if(mMesh.subMeshes[i].name==name){
				return mSubMeshes[i];
			}
		}

		return null;
	}

	public Bone getBone(String name){
		int i;
		for(i=0;i<mMesh.bones.length;++i){
			if(mMesh.bones[i].name==name){
				return mBones[i];
			}
		}

		return null;
	}

	public void setTexture(String textureName){
		Texture texture=mEngine.getTexture(textureName);

		int i;
		for(i=0;i<mSubMeshes.length;++i){
			if(mMesh.subMeshes[i].material.getTextureName()!=null){
				mSubMeshes[i].material=mMesh.subMeshes[i].material.clone();
				mSubMeshes[i].material.setTexture(texture);
			}
		}
	}

	public void setTexture(Texture texture){
		int i;
		for(i=0;i<mSubMeshes.length;++i){
			if(mMesh.subMeshes[i].material.getTextureName()!=null){
				mSubMeshes[i].material=mMesh.subMeshes[i].material.clone();
				mSubMeshes[i].material.setTexture(texture);
			}
		}
	}

	public void setTextures(String[] textureNames){
		int i;
		int numTextures=0;
		for(i=0;i<mSubMeshes.length;++i){
			if(mMesh.subMeshes[i].material.getTextureName()!=null){
				numTextures++;
			}
		}

		if(numTextures!=textureNames.length){
			throw new RuntimeException("Invalid number of texture names");
		}

		numTextures=0;
		for(i=0;i<mSubMeshes.length;++i){
			if(mMesh.subMeshes[i].material.getTextureName()!=null){
				mSubMeshes[i].material=mMesh.subMeshes[i].material.clone();
				mSubMeshes[i].material.setTexture(mEngine.getTexture(textureNames[numTextures]));
				numTextures++;
			}
		}
	}

	public void setTextures(Texture[] textures){
		int i;
		int numTextures=0;
		for(i=0;i<mSubMeshes.length;++i){
			if(mMesh.subMeshes[i].material.getTextureName()!=null){
				numTextures++;
			}
		}

		if(numTextures!=textures.length){
			throw new RuntimeException("Invalid number of texture names");
		}

		numTextures=0;
		for(i=0;i<mSubMeshes.length;++i){
			if(mMesh.subMeshes[i].material.getTextureName()!=null){
				mSubMeshes[i].material=mMesh.subMeshes[i].material.clone();
				mSubMeshes[i].material.setTexture(textures[numTextures]);
				numTextures++;
			}
		}
	}

	public void startAnimation(int animation,boolean loop){
		startAnimation(animation,loop,Math.ONE,false);
	}

	public void startAnimation(int animation,boolean loop,int timeScale,boolean keepLastFrame){
		if(mSceneManager==null){
			mSceneManager=mEngine.getSceneManager();
			mSceneManager.registerAnimatedMeshEntity(this);
		}

		mCurrentAnimation=animation;
		mAnimationTime=0;
		mLoopAnimation=loop;
		mAnimationTimeScale=timeScale;
		mKeepLastFrame=keepLastFrame;
		updateAnimation(0);
	}

	public void stopAnimation(){
		if(mSceneManager!=null){
			mSceneManager.unregisterAnimatedMeshEntity(this);
			mSceneManager=null;
		}

		mCurrentAnimation=-1;
		mAnimationTime=0;
		mNeedsUpdating=true;
	}

	void updateAnimation(int dt){
		if(mCurrentAnimation!=-1){
			mAnimationTime+=Math.fixedToInt(Math.mul(Math.intToFixed(dt),mAnimationTimeScale));
			if(mAnimationTime>=mMesh.animations[mCurrentAnimation].length){
				if(mLoopAnimation==true){
					mAnimationTime=0;
				}
				else if(mKeepLastFrame==true){
					mAnimationTime=mMesh.animations[mCurrentAnimation].length;
				}
				else{
					stopAnimation();
				}
			}
		}

		if(mBones!=null && mBones.length>0 && (mCurrentAnimation!=-1 || mNeedsUpdating)){
			int i;
			for(i=0;i<mBones.length;++i){
				if(mBones[i].parent==null){
					updateBone(mBones[i]);
				}
			}

			updateVertexBuffer();

			mNeedsUpdating=false;
		}
	}

	void createVertexBuffer(){
		if(mDynamicVertexData==null){
			if(mMesh.bones!=null && mMesh.bones.length>0){
				Buffer srcVertexBuffer=mMesh.staticVertexData.getVertexBuffer(0);
				VertexFormat vertexFormat=srcVertexBuffer.getVertexFormat();
				int numVertexes=srcVertexBuffer.getSize();
				Buffer vertexBuffer=new Buffer(Buffer.UT_DYNAMIC,Buffer.AT_WRITE_ONLY,vertexFormat,numVertexes);
				{
					java.nio.IntBuffer srcData=(java.nio.IntBuffer)srcVertexBuffer.lock(Buffer.LT_READ_ONLY);
					java.nio.IntBuffer dstData=(java.nio.IntBuffer)vertexBuffer.lock(Buffer.LT_WRITE_ONLY);

					int[] array=new int[srcData.limit()];
					srcData.get(array);
					dstData.put(array);

					vertexBuffer.unlock();
					srcVertexBuffer.unlock();
				}

				mDynamicVertexData=new VertexData(vertexBuffer);
			}
		}
	}

	void updateVertexBuffer(){
		if(mDynamicVertexData!=null){
			Buffer srcVertexBuffer=mMesh.staticVertexData.getVertexBuffer(0);
			Buffer dstVertexBuffer=mDynamicVertexData.getVertexBuffer(0);

			VertexFormat format=srcVertexBuffer.getVertexFormat();
			int formatSize=format.getVertexFormatSize();
			int positionFormatOffset=format.getVertexElementFromType(VertexElement.TYPE_POSITION).formatOffset;
			int normalFormatOffset=format.getVertexElementFromType(VertexElement.TYPE_NORMAL).formatOffset;

			{
				java.nio.IntBuffer srcData=(java.nio.IntBuffer)srcVertexBuffer.lock(Buffer.LT_READ_ONLY);
				java.nio.IntBuffer dstData=(java.nio.IntBuffer)dstVertexBuffer.lock(Buffer.LT_WRITE_ONLY);

				int offset;
				int i;
				for(i=0;i<srcVertexBuffer.getSize();++i){
					Bone bone=mBones[mMesh.boneControllingVertexes[i]];

					offset=i*formatSize + positionFormatOffset;
					mCacheVector3.set(srcData.get(offset+0),srcData.get(offset+1),srcData.get(offset+2));

					Math.mul(mCacheVector3,bone.boneSpaceRotate);
					Math.add(mCacheVector3,bone.boneSpaceTranslate);

					dstData.put(offset+0,mCacheVector3.x);
					dstData.put(offset+1,mCacheVector3.y);
					dstData.put(offset+2,mCacheVector3.z);

					offset=i*formatSize + normalFormatOffset;
					mCacheVector3.set(srcData.get(offset+0),srcData.get(offset+1),srcData.get(offset+2));

					Math.mul(mCacheVector3,bone.boneSpaceRotate);

					dstData.put(offset+0,mCacheVector3.x);
					dstData.put(offset+1,mCacheVector3.y);
					dstData.put(offset+2,mCacheVector3.z);
				}

				dstVertexBuffer.unlock();
				srcVertexBuffer.unlock();
			}
		}
	}

	boolean updateBoneTransformation(Bone bone){
		if(mCurrentAnimation!=-1){
			Animation animation=mMesh.animations[mCurrentAnimation];
			Animation.Track track=animation.getTrackFromBone(bone.boneIndex);

			if(track!=null){
				mCacheF1[0]=null;
				mCacheF2[0]=null;
				int t=track.getKeyFramesAtTime(Math.milliToFixed(mAnimationTime),mCacheF1,mCacheF2,mTrackHints[bone.boneIndex]);
				if(mCacheF1[0]==null || mCacheF2[0]==null){
					return false;
				}
				Math.setLerp(bone.localTranslate,mCacheF1[0].translate,mCacheF2[0].translate,t);
				Math.setLerp(bone.localRotate,mCacheF1[0].rotate,mCacheF2[0].rotate,t);
				Math.normalizeCarefully(bone.localRotate,0);
				return true;
			}
		}

		return false;
	}

	void updateBone(Bone bone){
		if(updateBoneTransformation(bone)==false){
			bone.localTranslate.set(mMesh.bones[bone.boneIndex].translate);
			bone.localRotate.set(mMesh.bones[bone.boneIndex].rotate);
		}

		Bone parent=bone.parent;
		if(parent==null){
			bone.worldRotate.set(bone.localRotate);
			bone.worldTranslate.set(bone.localTranslate);
		}
		else{
			Math.mul(bone.worldRotate,parent.worldRotate,bone.localRotate);
			Math.mul(mCacheVector3,parent.worldRotateMatrix,bone.localTranslate);
			Math.add(bone.worldTranslate,parent.worldTranslate,mCacheVector3);
		}

		Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);

		Math.mul(mCacheVector3,bone.worldRotateMatrix,mMesh.bones[bone.boneIndex].worldToBoneTranslate);
		Math.add(bone.boneSpaceTranslate,bone.worldTranslate,mCacheVector3);
		Math.mul(mCacheQuaternion,bone.worldRotate,mMesh.bones[bone.boneIndex].worldToBoneRotate);
		Math.setMatrix3x3FromQuaternion(bone.boneSpaceRotate,mCacheQuaternion);

		if(bone.children!=null){
			int i;
			for(i=0;i<bone.children.length;++i){
				updateBone(bone.children[i]);
			}
		}
	}

	Mesh mMesh;
	SubMesh[] mSubMeshes;

	Bone[] mBones;
	int mCurrentAnimation;
	int mAnimationTime;
	int mAnimationTimeScale;
	boolean mLoopAnimation;
	boolean mKeepLastFrame;
	boolean mNeedsUpdating;
	int[] mTrackHints;

	VertexData mDynamicVertexData;

	Vector3 mCacheVector3=new Vector3();
	Quaternion mCacheQuaternion=new Quaternion();
	Animation.KeyFrame[] mCacheF1=new Animation.KeyFrame[1];
	Animation.KeyFrame[] mCacheF2=new Animation.KeyFrame[1];

	SceneManager mSceneManager;
}
