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

package com.lightningtoads.toadlet.tadpole.sg{

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.sg.*;

public class MeshEntity extends RenderableEntity{
	public function MeshEntity(engine:Engine){
		super(engine);
		mType|=TYPE_MESH;

		mCurrentAnimation=-1;
		mAnimationTimeScale=tMath.ONE;
	}

	public override function destroy():void{
		super.destroy();

		stopAnimation();
	}

	public function loadS(name:String):void{
		loadM(mEngine.getMesh(name));
	}

	public function loadM(mesh:Mesh):void{
		mMesh=mesh;

		if(mMesh==null){
			throw new Error("Invalid Mesh");
		}

		var count:int=0;
		var i:int;
		for(i=0;i<mMesh.subMeshes.length;++i){
			if(mMesh.subMeshes[i].hasTexture){
				count++;
			}
		}

		mTextures=new Array(count);
		var bone:MeshEntityBone;

		if(mMesh.bones!=null && mMesh.bones.length>0){
			mBones=new Array();
			for(i=0;i<mMesh.bones.length;++i){
				bone=new MeshEntityBone();
				bone.boneIndex=i;
				mBones[i]=bone;
			}

			for(i=0;i<mBones.length;++i){
				bone=mBones[i];

				var parent:int=mMesh.bones[bone.boneIndex].parent;
				if(parent!=-1){
					bone.parent=mBones[parent];
				}
				else{
					bone.parent=null;
				}

				if(mMesh.bones[bone.boneIndex].children!=null){
					bone.children=new Array(mMesh.bones[bone.boneIndex].children.length);
					var j:int;
					for(j=0;j<mMesh.bones[bone.boneIndex].children.length;++j){
						bone.children[j]=mBones[mMesh.bones[bone.boneIndex].children[j]];
					}
				}
			}

			mTrackHints=new Array();

			createVertexBuffer();

			updateAnimation(0);
		}
	}

	public function setTexture(name:String):void{
	}

	public function setTextures(name:Array):void{
	}

	public function startAnimation(animation:int,loop:Boolean):void{
		startAnimationSK(animation,loop,tMath.ONE,false);
	}

	public function startAnimationSK(animation:int,loop:Boolean,timeScale:int,keepLastFrame:Boolean):void{
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

	public function stopAnimation():void{
		if(mSceneManager!=null){
			mSceneManager.unregisterAnimatedMeshEntity(this);
			mSceneManager=null;
		}

		mCurrentAnimation=-1;
		mAnimationTime=0;
		mNeedsUpdating=true;
	}

	internal function updateAnimation(dt:int):void{
		if(mCurrentAnimation!=-1){
			mAnimationTime+=tMath.fixedToInt(tMath.mul(tMath.intToFixed(dt),mAnimationTimeScale));
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
			var i:int;
			for(i=0;i<mBones.length;++i){
				if(mBones[i].parent==null){
					updateBone(mBones[i]);
				}
			}

			updateVertexBuffer();

			mNeedsUpdating=false;
		}
	}

	internal function createVertexBuffer():void{
		if(mDynamicVertexData==null){
			if(mMesh.bones!=null && mMesh.bones.length>0){
				mDynamicVertexData=new VertexData();
				if(mMesh.vertexData.positions!=null){
					mDynamicVertexData.positions=mMesh.vertexData.positions.concat();
				}
				if(mMesh.vertexData.normals!=null){
					mDynamicVertexData.normals=mMesh.vertexData.normals.concat();
				}
				mDynamicVertexData.colors=mMesh.vertexData.colors; // Colors are not animated
				mDynamicVertexData.texCoords=mMesh.vertexData.texCoords; // TexCoords are not animated
			}
		}
	}

	internal function updateVertexBuffer():void{
		if(mDynamicVertexData!=null){
			var srcPos:Array=mMesh.vertexData.positions;
			var dstPos:Array=mDynamicVertexData.positions;
			var srcNorm:Array=mMesh.vertexData.normals;
			var dstNorm:Array=mDynamicVertexData.normals;
			var offset:int;
			var m:Matrix3x3;
			var i:int;
			var len:int;
			var bone:MeshEntityBone;

			if(srcPos!=null && srcNorm!=null){
				len=srcPos.length/3;
				for(i=0;i<len;++i){
					bone=mBones[mMesh.boneControllingVertexes[i]];
					offset=i*3;

					m=bone.boneSpaceRotate;

					dstPos[offset+0]=
						// (m[0+0*3]*srcPos[offset+0]+m[0+1*3]*srcPos[offset+1]+m[0+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.x
						((((m.a00>>2)*(srcPos[offset+0]>>6))>>8)+(((m.a01>>2)*(srcPos[offset+1]>>6))>>8)+(((m.a02>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.x
					;
					dstPos[offset+1]=
						// (m[1+0*3]*srcPos[offset+0]+m[1+1*3]*srcPos[offset+1]+m[1+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.y
						((((m.a10>>2)*(srcPos[offset+0]>>6))>>8)+(((m.a11>>2)*(srcPos[offset+1]>>6))>>8)+(((m.a12>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.y
					;
					dstPos[offset+2]=
						// (m[2+0*3]*srcPos[offset+0]+m[2+1*3]*srcPos[offset+1]+m[2+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.z
						((((m.a20>>2)*(srcPos[offset+0]>>6))>>8)+(((m.a21>>2)*(srcPos[offset+1]>>6))>>8)+(((m.a22>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.z
					;

					dstNorm[offset+0]=
						// (m[0+0*3]*srcNorm[offset+0]+m[0+1*3]*srcNorm[offset+1]+m[0+2*3]*srcNorm[offset+2])
						((((m.a00>>2)*(srcNorm[offset+0]>>6))>>8)+(((m.a01>>2)*(srcNorm[offset+1]>>6))>>8)+(((m.a02>>2)*(srcNorm[offset+2]>>6))>>8))
					;
					dstNorm[offset+1]=
						// (m[1+0*3]*srcNorm[offset+0]+m[1+1*3]*srcNorm[offset+1]+m[1+2*3]*srcNorm[offset+2])
						((((m.a10>>2)*(srcNorm[offset+0]>>6))>>8)+(((m.a11>>2)*(srcNorm[offset+1]>>6))>>8)+(((m.a12>>2)*(srcNorm[offset+2]>>6))>>8))
					;
					dstNorm[offset+2]=
						// (m[2+0*3]*srcNorm[offset+0]+m[2+1*3]*srcNorm[offset+1]+m[2+2*3]*srcNorm[offset+2])
						((((m.a20>>2)*(srcNorm[offset+0]>>6))>>8)+(((m.a21>>2)*(srcNorm[offset+1]>>6))>>8)+(((m.a22>>2)*(srcNorm[offset+2]>>6))>>8))
					;
				}
			}
			else if(srcPos!=null){
				len=srcPos.length/3;
				for(i=0;i<len;++i){
					bone=mBones[mMesh.boneControllingVertexes[i]];
					offset=i*3;

					m=bone.boneSpaceRotate;

					dstPos[offset+0]=
						// (m[0+0*3]*srcPos[offset+0]+m[0+1*3]*srcPos[offset+1]+m[0+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.x
						((((m.a00>>2)*(srcPos[offset+0]>>6))>>8)+(((m.a01>>2)*(srcPos[offset+1]>>6))>>8)+(((m.a02>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.x
					;
					dstPos[offset+1]=
						// (m[1+0*3]*srcPos[offset+0]+m[1+1*3]*srcPos[offset+1]+m[1+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.y
						((((m.a10>>2)*(srcPos[offset+0]>>6))>>8)+(((m.a11>>2)*(srcPos[offset+1]>>6))>>8)+(((m.a12>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.y
					;
					dstPos[offset+2]=
						// (m[2+0*3]*srcPos[offset+0]+m[2+1*3]*srcPos[offset+1]+m[2+2*3]*srcPos[offset+2])+bone.boneSpaceTranslate.z
						((((m.a20>>2)*(srcPos[offset+0]>>6))>>8)+(((m.a21>>2)*(srcPos[offset+1]>>6))>>8)+(((m.a22>>2)*(srcPos[offset+2]>>6))>>8))+bone.boneSpaceTranslate.z
					;
				}
			}
		}
	}

	internal function updateBoneTransformation(bone:MeshEntityBone):Boolean{
		if(mCurrentAnimation!=-1){
			var animation:Animation=mMesh.animations[mCurrentAnimation];
			var track:AnimationTrack=animation.getTrackFromBone(bone.boneIndex);

			if(track!=null){
				mCacheF1[0]=null;
				mCacheF2[0]=null;
				var t:int=track.getKeyFramesAtTime(tMath.milliToFixed(mAnimationTime),mCacheF1,mCacheF2,mTrackHints[bone.boneIndex]);
				if(mCacheF1[0]==null || mCacheF2[0]==null){
					return false;
				}
				tMath.setLerpVVV(bone.localTranslate,mCacheF1[0].translate,mCacheF2[0].translate,t);
				tMath.setLerpQQQ(bone.localRotate,mCacheF1[0].rotate,mCacheF2[0].rotate,t);
				tMath.normalizeCarefullyQ(bone.localRotate,0);
				return true;
			}
		}

		return false;
	}

	internal function updateBone(bone:MeshEntityBone):void{
		if(updateBoneTransformation(bone)==false){
			bone.localTranslate.setV(mMesh.bones[bone.boneIndex].translate);
			bone.localRotate.setQ(mMesh.bones[bone.boneIndex].rotate);
		}

		var parent:MeshEntityBone=bone.parent;
		if(parent==null){
			bone.worldRotate.setQ(bone.localRotate);
			bone.worldTranslate.setV(bone.localTranslate);
		}
		else{
			tMath.mulQQQ(bone.worldRotate,parent.worldRotate,bone.localRotate);
			tMath.mulVMV(mCacheVector3,parent.worldRotateMatrix,bone.localTranslate);
			tMath.addVVV(bone.worldTranslate,parent.worldTranslate,mCacheVector3);
		}

		tMath.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);

		tMath.mulVMV(mCacheVector3,bone.worldRotateMatrix,mMesh.bones[bone.boneIndex].worldToBoneTranslate);
		tMath.addVVV(bone.boneSpaceTranslate,bone.worldTranslate,mCacheVector3);
		tMath.mulQQQ(mCacheQuaternion,bone.worldRotate,mMesh.bones[bone.boneIndex].worldToBoneRotate);
		tMath.setMatrix3x3FromQuaternion(bone.boneSpaceRotate,mCacheQuaternion);

		if(bone.children!=null){
			var i:int;
			for(i=0;i<bone.children.length;++i){
				updateBone(bone.children[i]);
			}
		}
	}

	public var mMesh:Mesh;

	internal var mBones:Array;
	internal var mCurrentAnimation:int;
	internal var mAnimationTime:int;
	internal var mAnimationTimeScale:int;
	internal var mLoopAnimation:Boolean;
	internal var mKeepLastFrame:Boolean;
	internal var mNeedsUpdating:Boolean;
	internal var mTrackHints:Array;

	public var mDynamicVertexData:VertexData;

	public var mTextures:Array=new Array();
	
	internal var mCacheVector3:Vector3=new Vector3();
	internal var mCacheQuaternion:Quaternion=new Quaternion();
	internal var mCacheF1:Array=new Array();
	internal var mCacheF2:Array=new Array();

	internal var mSceneManager:SceneManager;
}

}
