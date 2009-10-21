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

	static final int TEXTURE_SHIFT_BITS=9;

	public MeshEntity(Engine engine){
		super(engine);
		mType|=TYPE_MESH;

		mCurrentAnimation=-1;
		mAnimationTimeScale=Math.ONE;
	}

	public void destroy(){
		super.destroy();

		stopAnimation();
	}

	public void load(String name){
		load(mEngine.getMesh(name));
	}

	public void load(Mesh mesh){
		mMesh=mesh;

		if(mMesh==null){
			throw new RuntimeException("Invalid Mesh");
		}

		int count=0;
		int i;
		for(i=0;i<mMesh.subMeshes.length;++i){
			if(mMesh.subMeshes[i].hasTexture){
				count++;
			}
		}

		mTextures=new int[count][][];
		mTexCoordShiftBits=new byte[count];

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
	}

	public void setTexture(String name){
		setTexture(mEngine.getTexture(name));
	}

	public void setTexture(int[][] texture){
		int i;
		if(texture!=null){
			for(i=0;i<mTextures.length;++i){
				mTextures[i]=texture;

				if(texture[0][0]>256 || texture[0][1]>256){
					throw new RuntimeException("Texture size > 256");
				}

				int size=texture[0][0];
				byte bits=TEXTURE_SHIFT_BITS;
				while(size>0){size>>=1;bits--;}
				mTexCoordShiftBits[i]=bits;
			}
		}
		else{
			for(i=0;i<mTextures.length;++i){
				mTextures[i]=null;
			}
		}
	}

	public void setTextures(String[] names){
		int i;
		if(names!=null){
			int[][][] textures=new int[names.length][][];
			for(i=0;i<names.length;++i){
				textures[i]=mEngine.getTexture(names[i]);
			}
			setTextures(textures);
		}
		else{
			setTextures((int[][][])null);
		}
	}

	public void setTextures(int[][][] textures){
		if(textures!=null){
			if(textures.length!=mTextures.length){
				throw new RuntimeException("Texture names & SubMesh count mismatch");
			}

			int i;
			for(i=0;i<textures.length;++i){
				mTextures[i]=textures[i];

				if(textures[i][0][0]>256 || textures[i][0][1]>256){
					throw new RuntimeException("Texture size > 256");
				}

				int size=textures[i][0][0];
				byte bits=TEXTURE_SHIFT_BITS;
				while(size>0){size>>=1;bits--;}
				mTexCoordShiftBits[i]=bits;
			}
		}
		else{
			int i;
			for(i=0;i<mTextures.length;++i){
				mTextures[i]=null;
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
				mDynamicVertexData=new VertexData();
				if(mMesh.vertexData.positions!=null){
					mDynamicVertexData.positions=new int[mMesh.vertexData.positions.length];
					System.arraycopy(mMesh.vertexData.positions,0,mDynamicVertexData.positions,0,mMesh.vertexData.positions.length);
				}
				if(mMesh.vertexData.normals!=null){
					mDynamicVertexData.normals=new int[mMesh.vertexData.normals.length];
					System.arraycopy(mMesh.vertexData.normals,0,mDynamicVertexData.normals,0,mMesh.vertexData.normals.length);
				}
				mDynamicVertexData.colors=mMesh.vertexData.colors; // Colors are not animated
				mDynamicVertexData.texCoords=mMesh.vertexData.texCoords; // TexCoords are not animated
			}
		}
	}

	void updateVertexBuffer(){
		if(mDynamicVertexData!=null){
			int[] srcPos=mMesh.vertexData.positions;
			int[] dstPos=mDynamicVertexData.positions;
			int[] srcNorm=mMesh.vertexData.normals;
			int[] dstNorm=mDynamicVertexData.normals;
			int offset;
			int[] m;
			int i;

			if(srcPos!=null && srcNorm!=null){
				int len=srcPos.length/3;
				for(i=0;i<len;++i){
					Bone bone=mBones[mMesh.boneControllingVertexes[i]];
					offset=i*3;

					m=bone.boneSpaceRotate.data;

					dstPos[offset+0]=
						////#exec fpcomp.bat  "(m[0+0*3]*srcPos[offset+0]+m[0+1*3]*srcPos[offset+1]+m[0+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.x"
						((((m[0+0*3]>>2)*(srcPos[offset+0]>>6))>>8)+(((m[0+1*3]>>2)*(srcPos[offset+1]>>6))>>8)+(((m[0+2*3]>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.x
					;
					dstPos[offset+1]=
						////#exec fpcomp.bat  "(m[1+0*3]*srcPos[offset+0]+m[1+1*3]*srcPos[offset+1]+m[1+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.y"
						((((m[1+0*3]>>2)*(srcPos[offset+0]>>6))>>8)+(((m[1+1*3]>>2)*(srcPos[offset+1]>>6))>>8)+(((m[1+2*3]>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.y
					;
					dstPos[offset+2]=
						////#exec fpcomp.bat  "(m[2+0*3]*srcPos[offset+0]+m[2+1*3]*srcPos[offset+1]+m[2+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.z"
						((((m[2+0*3]>>2)*(srcPos[offset+0]>>6))>>8)+(((m[2+1*3]>>2)*(srcPos[offset+1]>>6))>>8)+(((m[2+2*3]>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.z
					;

					dstNorm[offset+0]=
						////#exec fpcomp.bat  "(m[0+0*3]*srcNorm[offset+0]+m[0+1*3]*srcNorm[offset+1]+m[0+2*3]*srcNorm[offset+2])"
						((((m[0+0*3]>>2)*(srcNorm[offset+0]>>6))>>8)+(((m[0+1*3]>>2)*(srcNorm[offset+1]>>6))>>8)+(((m[0+2*3]>>2)*(srcNorm[offset+2]>>6))>>8))
					;
					dstNorm[offset+1]=
						////#exec fpcomp.bat  "(m[1+0*3]*srcNorm[offset+0]+m[1+1*3]*srcNorm[offset+1]+m[1+2*3]*srcNorm[offset+2])"
						((((m[1+0*3]>>2)*(srcNorm[offset+0]>>6))>>8)+(((m[1+1*3]>>2)*(srcNorm[offset+1]>>6))>>8)+(((m[1+2*3]>>2)*(srcNorm[offset+2]>>6))>>8))
					;
					dstNorm[offset+2]=
						////#exec fpcomp.bat  "(m[2+0*3]*srcNorm[offset+0]+m[2+1*3]*srcNorm[offset+1]+m[2+2*3]*srcNorm[offset+2])"
						((((m[2+0*3]>>2)*(srcNorm[offset+0]>>6))>>8)+(((m[2+1*3]>>2)*(srcNorm[offset+1]>>6))>>8)+(((m[2+2*3]>>2)*(srcNorm[offset+2]>>6))>>8))
					;
				}
			}
			else if(srcPos!=null){
				int len=srcPos.length/3;
				for(i=0;i<len;++i){
					Bone bone=mBones[mMesh.boneControllingVertexes[i]];
					offset=i*3;

					m=bone.boneSpaceRotate.data;

					dstPos[offset+0]=
						////#exec fpcomp.bat  "(m[0+0*3]*srcPos[offset+0]+m[0+1*3]*srcPos[offset+1]+m[0+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.x"
						((((m[0+0*3]>>2)*(srcPos[offset+0]>>6))>>8)+(((m[0+1*3]>>2)*(srcPos[offset+1]>>6))>>8)+(((m[0+2*3]>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.x
					;
					dstPos[offset+1]=
						////#exec fpcomp.bat  "(m[1+0*3]*srcPos[offset+0]+m[1+1*3]*srcPos[offset+1]+m[1+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.y"
						((((m[1+0*3]>>2)*(srcPos[offset+0]>>6))>>8)+(((m[1+1*3]>>2)*(srcPos[offset+1]>>6))>>8)+(((m[1+2*3]>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.y
					;
					dstPos[offset+2]=
						////#exec fpcomp.bat  "(m[2+0*3]*srcPos[offset+0]+m[2+1*3]*srcPos[offset+1]+m[2+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.z"
						((((m[2+0*3]>>2)*(srcPos[offset+0]>>6))>>8)+(((m[2+1*3]>>2)*(srcPos[offset+1]>>6))>>8)+(((m[2+2*3]>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.z
					;
				}
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

	public Mesh mMesh;

	public int[][][] mTextures;
	public byte[] mTexCoordShiftBits;

	Bone[] mBones;
	int mCurrentAnimation;
	int mAnimationTime;
	int mAnimationTimeScale;
	boolean mLoopAnimation;
	boolean mKeepLastFrame;
	boolean mNeedsUpdating;
	int[] mTrackHints;

	public VertexData mDynamicVertexData;

	Vector3 mCacheVector3=new Vector3();
	Quaternion mCacheQuaternion=new Quaternion();
	Animation.KeyFrame[] mCacheF1=new Animation.KeyFrame[1];
	Animation.KeyFrame[] mCacheF2=new Animation.KeyFrame[1];

	SceneManager mSceneManager;
}
