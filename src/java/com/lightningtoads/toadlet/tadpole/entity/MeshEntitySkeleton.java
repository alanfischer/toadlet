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

import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;

public class MeshEntitySkeleton implements Renderable{
	public class BoneSpaceUpdate{
		public final static int NONE=			0;
		public final static int FLAG_TRANSLATE=	1<<0;
		public final static int FLAG_ROTATE=	1<<1;
		public final static int FLAG_MATRIX=	1<<2;
		public final static int FLAG_WORLD=		1<<3;
	}

	public interface BoneController{
		public int updateBoneTransform(Bone bone);
	};

	public class Bone{
		public Bone(int index){
			this.index=index;
		}

		public void setOverrideTranslate(Vector3 translate){
			localTranslate.set(translate);
			dontUpdateFlags|=BoneSpaceUpdate.FLAG_TRANSLATE;
		}

		public void setOverrideRotate(Quaternion rotate){
			localRotate.set(rotate);
			dontUpdateFlags|=BoneSpaceUpdate.FLAG_ROTATE;
		}

		public void removeOverrides(){dontUpdateFlags=0;}

		public int index=0;

		public Vector3 localTranslate=new Vector3();
		public Quaternion localRotate=new Quaternion();

		public Vector3 worldTranslate=new Vector3();
		public Quaternion worldRotate=new Quaternion();
		public Matrix3x3 worldRotateMatrix=new Matrix3x3();

		public Vector3 boneSpaceTranslate=new Vector3();
		public Matrix3x3 boneSpaceRotate=new Matrix3x3();

		public BoneController controller=null;

		public int dontUpdateFlags=0;

		public boolean useMatrixTransforms=false;
	};

	public MeshEntitySkeleton(MeshEntity entity,MeshSkeleton skeleton){
		mEntity=entity;
		mSkeleton=skeleton;

		mBones=new Bone[skeleton.bones.length];
		int i;
		for(i=0;i<skeleton.bones.length;++i){
			mBones[i]=new Bone(i);
		}

		mTrackHints=new int[mBones.length];
		
		updateBones();
	}

	public void updateBones(){
		int i;
		for(i=0;i<mBones.length;++i){
			updateBone(mBones[i]);
		}

		if(mVertexData!=null){
			updateVertexData();
		}

		if(mEntity!=null && mEntity.getEngine().getScene()!=null){
			mLastVisualUpdateFrame=mEntity.getEngine().getScene().getVisualFrame();
		}
		else{
			mLastVisualUpdateFrame=-1;
		}
	}

	public void updateBones(int sequenceIndex,scalar sequenceTime){
		if(sequenceIndex>=0 && sequenceIndex<mSkeleton.sequences.size()){
			mSequence=mSkeleton.sequences.get(sequenceIndex);
			mSequenceTime=sequenceTime;
		}

		updateBones();
	}

	public int getLastVisualUpdateFrame(){return mLastVisualUpdateFrame;}

	public int getNumBones(){return mBones.length;}
	public Bone getBone(int index){return mBones[index];}

	public Bone getBone(String name){
		int i;
		for(i=0;i<mSkeleton.bones.length;++i){
			if(mSkeleton.bones[i].name.equals(name)){
				return mBones[i];
			}
		}

		return null;
	}

	public String getBoneName(int index){
		if(index>=0 && index<mSkeleton.bones.length){
			return mSkeleton.bones[index].name;
		}
		else{
			return null;
		}
	}
	
	public String getBoneName(Bone bone){
		return mSkeleton.bones[bone.index].name;
	}

	public MeshSkeleton getSkeleton(){return mSkeleton;}

	public int updateBoneTransformation(Bone bone){
		if(mSequence!=null){
			Track track=mSequence.tracks[bone.index];
			scalar time=mSequenceTime;
			if(track!=null){
				KeyFrame[] f1=cache_updateBoneTransformation_f1,f2=cache_updateBoneTransformation_f2;
				scalar t=track.getKeyFramesAtTime(time,f1,f2,mTrackHints,bone.index);
				if(f1==null || f2==null){
					return bone.dontUpdateFlags;
				}
				if((bone.dontUpdateFlags&BoneSpaceUpdate.FLAG_TRANSLATE)==0){
					Math.lerp(bone.localTranslate,f1[0].translate,f2[0].translate,t);
				}
				if((bone.dontUpdateFlags&BoneSpaceUpdate.FLAG_ROTATE)==0){
					Math.lerp(bone.localRotate,f1[0].rotate,f2[0].rotate,t);
					Math.normalizeCarefully(bone.localRotate,0);
				}
				return BoneSpaceUpdate.FLAG_TRANSLATE|BoneSpaceUpdate.FLAG_ROTATE;
			}
		}

		return bone.dontUpdateFlags;
	}

	public void updateBone(Bone bone){
		int updateFlags=BoneSpaceUpdate.NONE;

		MeshSkeleton.Bone sbone=mSkeleton.bones[bone.index];

		if(bone.controller==null){
			updateFlags=updateBoneTransformation(bone);
		}
		else{
			updateFlags=bone.controller.updateBoneTransform(bone);
		}

		// useMatrixTransforms forces the bones to do their transform calculations without quaternions
		// This is necessary if we have a bone controller that does not calculate the local or world quaternions
		bone.useMatrixTransforms=((updateFlags&BoneSpaceUpdate.FLAG_MATRIX)>0);

		Bone parent=null;
		int parentIndex=sbone.parentIndex;
		if(parentIndex>=0){
			parent=mBones[parentIndex];

			// If a parent only uses matrix transforms, then this bone must do it also, since the quaternions are not available
			bone.useMatrixTransforms|=parent.useMatrixTransforms;
		}

		Vector3 vector=cache_updateBone_vector.reset();
		Matrix3x3 matrix=cache_updateBone_matrix.reset();
		Quaternion quaternion=cache_updateBone_quaternion.reset();

		if((updateFlags&BoneSpaceUpdate.FLAG_WORLD)==0){
			if((updateFlags&BoneSpaceUpdate.FLAG_ROTATE)==0){
				bone.localRotate.set(sbone.rotate);
			}

			if((updateFlags&BoneSpaceUpdate.FLAG_TRANSLATE)==0){
				bone.localTranslate.set(sbone.translate);
			}

			if(bone.useMatrixTransforms){
				if(parent==null){
					Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.localRotate);

					bone.worldTranslate.set(bone.localTranslate);
				}
				else{
					Math.setMatrix3x3FromQuaternion(matrix,bone.localRotate);
					Math.mul(bone.worldRotateMatrix,parent.worldRotateMatrix,matrix);

					Math.mul(vector,parent.worldRotateMatrix,bone.localTranslate);
					Math.add(bone.worldTranslate,parent.worldTranslate,vector);
				}
			}
			else{
				if(parent==null){
					bone.worldRotate.set(bone.localRotate);
					Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);

					bone.worldTranslate.set(bone.localTranslate);
				}
				else{
					Math.mul(bone.worldRotate,parent.worldRotate,bone.localRotate);
					Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);

					Math.mul(vector,parent.worldRotateMatrix,bone.localTranslate);
					Math.add(bone.worldTranslate,parent.worldTranslate,vector);
				}
			}
		}
		else{
			if(bone.useMatrixTransforms){
				if(parent==null){
					if((updateFlags&BoneSpaceUpdate.FLAG_ROTATE)==0){
						Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,sbone.rotate);
					}
					else{
						if((updateFlags&BoneSpaceUpdate.FLAG_MATRIX)==0){
							Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);
						}
					}

					if((updateFlags&BoneSpaceUpdate.FLAG_TRANSLATE)==0){
						bone.worldTranslate.set(sbone.translate);
					}
				}
				else{
					if((updateFlags&BoneSpaceUpdate.FLAG_ROTATE)==0){
						Math.setMatrix3x3FromQuaternion(matrix,sbone.rotate);
						Math.mul(bone.worldRotateMatrix,parent.worldRotateMatrix,matrix);
					}
					else{
						if((updateFlags&BoneSpaceUpdate.FLAG_MATRIX)==0){
							Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);
						}
					}

					if((updateFlags&BoneSpaceUpdate.FLAG_TRANSLATE)==0){
						Math.mul(vector,parent.worldRotateMatrix,sbone.translate);
						Math.add(bone.worldTranslate,parent.worldTranslate,vector);
					}
				}
			}
			else{
				if(parent==null){
					if((updateFlags&BoneSpaceUpdate.FLAG_ROTATE)==0){
						bone.worldRotate.set(sbone.rotate);
					}
					Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);

					if((updateFlags&BoneSpaceUpdate.FLAG_TRANSLATE)==0){
						bone.worldTranslate.set(sbone.translate);
					}
				}
				else{
					if((updateFlags&BoneSpaceUpdate.FLAG_ROTATE)==0){
						Math.mul(bone.worldRotate,parent.worldRotate,sbone.rotate);
					}
					Math.setMatrix3x3FromQuaternion(bone.worldRotateMatrix,bone.worldRotate);

					if((updateFlags&BoneSpaceUpdate.FLAG_TRANSLATE)==0){
						Math.mul(vector,parent.worldRotateMatrix,sbone.translate);
						Math.add(bone.worldTranslate,parent.worldTranslate,vector);
					}
				}
			}
		}

		Math.mul(vector,bone.worldRotateMatrix,sbone.worldToBoneTranslate);
		Math.add(bone.boneSpaceTranslate,bone.worldTranslate,vector);

		if(bone.useMatrixTransforms){
			Math.setMatrix3x3FromQuaternion(matrix,sbone.worldToBoneRotate);
			Math.mul(bone.boneSpaceRotate,bone.worldRotateMatrix,matrix);
		}
		else{
			Math.mul(quaternion,bone.worldRotate,sbone.worldToBoneRotate);
			Math.setMatrix3x3FromQuaternion(bone.boneSpaceRotate,quaternion);
		}
	}

	public void setRenderable(boolean renderable){
		if(renderable){
			int i;
			mMaterial=new Material();
			mMaterial.setLighting(true);
			mMaterial.setLightEffect(new LightEffect(Colors.RED));
			mMaterial.setDepthTest(Renderer.DepthTest.NONE);

			IndexBuffer indexBuffer=new IndexBuffer(Buffer.UsageType.STATIC,Buffer.AccessType.WRITE_ONLY,IndexBuffer.IndexFormat.UINT_16,(mBones.length-1)*2);
			IndexBufferAccessor iba=new IndexBufferAccessor();
			{
				iba.lock(indexBuffer,Buffer.LockType.WRITE_ONLY);
				for(i=1;i<mBones.length;++i){
					iba.set((i-1)*2+0,mSkeleton.bones[i].parentIndex<0?0:mSkeleton.bones[i].parentIndex);
					iba.set((i-1)*2+1,i);
				}
				iba.unlock();
			}
			mIndexData=new IndexData(IndexData.Primitive.LINES,indexBuffer);

			VertexFormat vertexFormat=new VertexFormat(1);
			vertexFormat.addVertexElement(new VertexElement(VertexElement.Type.POSITION,VertexElement.Format.BIT_FLOAT_32|VertexElement.Format.BIT_COUNT_3));
			VertexBuffer vertexBuffer=new VertexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,vertexFormat,mBones.length);
			mVertexData=new VertexData(vertexBuffer);
			
			updateVertexData();
		}
		else{
			mMaterial=null;
			mIndexData=null;
			mVertexData=null;
		}
	}

	public void updateVertexData(){
		VertexBufferAccessor vba=new VertexBufferAccessor();
		vba.lock(mVertexData.getVertexBuffer(0),Buffer.LockType.WRITE_ONLY);
		int i;
		for(i=0;i<mBones.length;++i){
			vba.set3(i,0,mBones[i].worldTranslate);
		}
		vba.unlock();
	}
	
	public Matrix4x4 getRenderTransform(){
		return mEntity!=null?mEntity.getVisualWorldTransform():Math.IDENTITY_MATRIX4X4;
	}

	public Material getRenderMaterial(){return mMaterial;}

	public void render(Renderer renderer){
		renderer.renderPrimitive(mVertexData,mIndexData);
	}

	protected MeshEntity mEntity=null;
	protected MeshSkeleton mSkeleton=null;
	protected Bone[] mBones=null;
	protected int mLastVisualUpdateFrame=-1;

	protected MeshSkeletonSequence mSequence=null;
	protected scalar mSequenceTime=0;
	protected int[] mTrackHints=null;

	// For rendering the skeleton outline
	protected Material mMaterial=null;
	protected IndexData mIndexData=null;
	protected VertexData mVertexData=null;

	protected Vector3 cache_updateBone_vector=new Vector3();
	protected Matrix3x3 cache_updateBone_matrix=new Matrix3x3();
	protected Quaternion cache_updateBone_quaternion=new Quaternion();
	protected KeyFrame[] cache_updateBoneTransformation_f1={new KeyFrame()},cache_updateBoneTransformation_f2={new KeyFrame()};
}
