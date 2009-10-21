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
import com.lightningtoads.toadlet.tadpole.animation.*;

public class MeshEntity extends RenderableEntity{
	public class SubMesh implements Renderable{
		public SubMesh(MeshEntity meshEntity,Mesh.SubMesh meshSubMesh){
			this.meshEntity=meshEntity;
			this.meshSubMesh=meshSubMesh;
		}

		public Matrix4x4 getRenderTransform(){return meshEntity.getVisualWorldTransform();}
		public Material getRenderMaterial(){return material;}
		public void render(Renderer renderer){renderer.renderPrimitive(vertexData,indexData);}

		public Material material=null;
		public IndexData indexData=null;
		public VertexData vertexData=null;
		public MeshEntity meshEntity=null;
		public Mesh.SubMesh meshSubMesh=null;
	}

	/// Specialization of the AnimationController that allows for easy access to playing single sequences.
	public class MeshAnimationController extends AnimationController{
		public MeshAnimationController(MeshEntity entity){
			mMeshEntity=entity;
			mAnimation=new SkeletonAnimation();
			attachAnimation(mAnimation);

			mAnimation.setTarget(mMeshEntity.getSkeleton());
		}

		public void setSequenceIndex(int index){
			mAnimation.setSequenceIndex(index);
		}

		public int getSequenceIndex(){
			return mAnimation.getSequenceIndex();
		}

		public void start(){
			if(isRunning()){
				stop();
			}

			mMeshEntity.setReceiveUpdates(true);

			super.start();
			if(mMeshEntity.getEngine().getScene()!=null){
				mStartingFrame=mMeshEntity.getEngine().getScene().getLogicFrame();
			}
		}

		public void stop(){
			if(isRunning()==false){
				return;
			}

			mMeshEntity.setReceiveUpdates(false);

			super.stop();
		}

		public void logicUpdate(int dt){
			if(mMeshEntity.getEngine().getScene()==null || mStartingFrame!=mMeshEntity.getEngine().getScene().getLogicFrame()){
				super.logicUpdate(dt);
			}
		}

		public void visualUpdate(int dt){
			super.visualUpdate(dt);
		}

		public void skeletonChanged(){
			mAnimation.setTarget(mMeshEntity.getSkeleton());
		}
		
		protected MeshEntity mMeshEntity=null;
		protected SkeletonAnimation mAnimation=null;
		protected int mStartingFrame=0;
	}

	public MeshEntity(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		mMesh=null;
		mSubMeshes=null;
		mSkeleton=null;
		mDynamicVertexData=null;

		mAnimationController=null;

		return this;
	}

	public void destroy(){
		if(mAnimationController!=null){
			mAnimationController.stop();
			mAnimationController=null;
		}

		super.destroy();
	}

	public void load(String name){
		load(mEngine.getMesh(name));
	}

	public void load(Mesh mesh){
		int i;
		mSubMeshes=null;

		mMesh=mesh;

		if(mMesh==null){
			Error.invalidParameters(Categories.TOADLET_TADPOLE,
				"Invalid Mesh");
			return;
		}

		if(mesh.boundingRadius!=Math.ONE){
			mBoundingRadius=Math.mul(mesh.worldScale,mesh.boundingRadius);
		}
		else{
			mBoundingRadius=-Math.ONE;
		}

		if(mesh.skeleton!=null){
			mSkeleton=new MeshEntitySkeleton(this,mesh.skeleton);

			createVertexBuffer();
		}

		mSubMeshes=new SubMesh[mMesh.subMeshes.length];
		for(i=0;i<mMesh.subMeshes.length;++i){
			SubMesh subMesh=new SubMesh(this,mMesh.subMeshes[i]);
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

	public Mesh getMesh(){return mMesh;}

	public int getNumSubMeshes(){return mSubMeshes.length;}
	public SubMesh getSubMesh(int i){return mSubMeshes[i];}

	public SubMesh getSubMesh(String name){
		if(mMesh==null){
			return null;
		}

		int i;
		for(i=0;i<mMesh.subMeshes.length;++i){
			if(mMesh.subMeshes[i].name.equals(name)){
				return mSubMeshes[i];
			}
		}

		return null;
	}
		
	public MeshEntitySkeleton getSkeleton(){return mSkeleton;}

	public void setSkeleton(MeshEntitySkeleton skeleton){
		mSkeleton=skeleton;

		if(mAnimationController!=null){
			mAnimationController.skeletonChanged();
		}
	}

	public MeshAnimationController getAnimationController(){
		if(mAnimationController==null){
			mAnimationController=new MeshAnimationController(this);
		}

		return mAnimationController;
	}

	public void logicUpdate(int dt){
		if(mAnimationController!=null){
			mAnimationController.logicUpdate(dt);
		}
	}
	
	public void visualUpdate(int dt){
		if(mAnimationController!=null){
			mAnimationController.visualUpdate(dt);
		}
	}

	public void queueRenderables(Scene scene){
		if(mMesh!=null && mMesh.worldScale!=Math.ONE){
			mVisualWorldTransform.reset();
			mVisualWorldTransform.setAt(0,0,mMesh.worldScale);
			mVisualWorldTransform.setAt(1,1,mMesh.worldScale);
			mVisualWorldTransform.setAt(2,2,mMesh.worldScale);
			Math.preMul(mVisualWorldTransform,mVisualTransform);
			Math.preMul(mVisualWorldTransform,mParent.getVisualWorldTransform());
		}

		if(mSkeleton!=null){
			int lastVisualUpdateFrame=mSkeleton.getLastVisualUpdateFrame();
			if(lastVisualUpdateFrame==-1 || lastVisualUpdateFrame==scene.getVisualFrame()){
				updateVertexBuffer();
			}
		}

		int i;
		for(i=0;i<mSubMeshes.length;++i){
			scene.queueRenderable(mSubMeshes[i]);
		}

		if(mSkeleton!=null && mSkeleton.getRenderMaterial()!=null){
			scene.queueRenderable(mSkeleton);
		}
	}

	VertexBufferAccessor svba=new VertexBufferAccessor();
	VertexBufferAccessor dvba=new VertexBufferAccessor();

	public void createVertexBuffer(){
		if(mDynamicVertexData==null){
			if(mSkeleton!=null){
				VertexBuffer srcVertexBuffer=mMesh.staticVertexData.getVertexBuffer(0);
				VertexFormat vertexFormat=srcVertexBuffer.getVertexFormat();
				int numVertexes=srcVertexBuffer.getSize();
				VertexBuffer vertexBuffer=new VertexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,vertexFormat,numVertexes);
				vertexBuffer.setRememberContents(false);
				if(mEngine!=null){ // The NULL check is so we can call this method from the mesh compilers, when the MeshEntity is created without an Engine
					vertexBuffer=mEngine.loadVertexBuffer(vertexBuffer);
				}

				java.nio.ByteBuffer srcData=srcVertexBuffer.lock(Buffer.LockType.READ_ONLY);
				java.nio.ByteBuffer dstData=vertexBuffer.lock(Buffer.LockType.WRITE_ONLY);
				dstData.put(srcData);
				srcData.rewind();
				dstData.rewind();
				vertexBuffer.unlock();
				srcVertexBuffer.unlock();

				mDynamicVertexData=new VertexData(vertexBuffer);
			}
		}
	}
	
	public void updateVertexBuffer(){
		if(mDynamicVertexData!=null){
			VertexBuffer srcVertexBuffer=mMesh.staticVertexData.getVertexBuffer(0);
			VertexBuffer dstVertexBuffer=mDynamicVertexData.getVertexBuffer(0);

			VertexFormat format=srcVertexBuffer.getVertexFormat();
			int positionElement=format.getVertexElementIndexOfType(VertexElement.Type.POSITION);

			{
				svba.lock(srcVertexBuffer,Buffer.LockType.READ_ONLY);
				dvba.lock(dstVertexBuffer,Buffer.LockType.WRITE_ONLY);

				// It appears that hardware buffers in opengl don't store their previous data if you want to write to them.
				// Basically, if you want to write any of it, you have to write it all.
				// So we re-copy over all the data to start
				// NOTE: Probably due to UT_WRITE_ONLY on creation, makes it mean if we lock it, we will rewrite all that data
				// TODO: Research this more, and see if this is a known problem
				dvba.getData().put(svba.getData());
				svba.getData().rewind();
				dvba.getData().rewind();

				Vector3 vector=cache_updateVertexBuffer_vector.reset();

				int offset;
				int i;
				if(format.hasVertexElementOfType(VertexElement.Type.NORMAL)){
					int normalElement=format.getVertexElementIndexOfType(VertexElement.Type.NORMAL);
					for(i=0;i<srcVertexBuffer.getSize();++i){
						MeshEntitySkeleton.Bone bone=mSkeleton.getBone(mMesh.vertexBoneAssignments[i][0].bone); // TODO: Implement all bones

						svba.get3(i,positionElement,vector);

						Math.mul(vector,bone.boneSpaceRotate);
						Math.add(vector,bone.boneSpaceTranslate);

						dvba.set3(i,positionElement,vector);

						svba.get3(i,normalElement,vector);

						Math.mul(vector,bone.boneSpaceRotate);

						dvba.set3(i,normalElement,vector);
					}
				}
				else{
					for(i=0;i<srcVertexBuffer.getSize();++i){
						MeshEntitySkeleton.Bone bone=mSkeleton.getBone(mMesh.vertexBoneAssignments[i][0].bone); // TODO: Implement all bones

						svba.get3(i,positionElement,vector);

						Math.mul(vector,bone.boneSpaceRotate);
						Math.add(vector,bone.boneSpaceTranslate);

						dvba.set3(i,positionElement,vector);
					}
				}

				svba.unlock();
				dvba.unlock();
			}
		}
	}
	
	protected Mesh mMesh=null;
	protected SubMesh[] mSubMeshes=null;
	protected MeshEntitySkeleton mSkeleton=null;
	protected VertexData mDynamicVertexData=null;

	protected MeshAnimationController mAnimationController=null;

	protected Vector3 cache_updateVertexBuffer_vector=new Vector3();
}
