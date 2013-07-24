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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/MeshComponent.h>

namespace toadlet{
namespace tadpole{

MeshComponent::SubMesh::SubMesh(MeshComponent *parent,Mesh::SubMesh *meshSubMesh):
	mScope(-1)
{
	mParent=parent;
	mMeshSubMesh=meshSubMesh;
	mIndexData=meshSubMesh->indexData;
	mVertexData=meshSubMesh->vertexData;
	mMaterial=meshSubMesh->material;
}

void MeshComponent::SubMesh::render(RenderManager *manager) const{
	manager->getDevice()->renderPrimitive(mVertexData,mIndexData);
}

MeshComponent::MeshComponent(Engine *engine):
	mRendered(true)
	//mMesh,
	//mSubMeshes,
	//mSkeleton,
	//mDynamicVertexData
{
	mEngine=engine;
	mTransform=new Transform();
	mTransform->setTransformListener(this);
	mBound=new Bound();
	mWorldTransform=new Transform();
	mWorldBound=new Bound();
}

void MeshComponent::destroy(){
	mSubMeshes.clear();

	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	if(mDynamicVertexData!=NULL){
		mDynamicVertexData->destroy();
		mDynamicVertexData=NULL;
	}

	if(mSkeleton!=NULL){
		mSkeleton->destroy();
		mSkeleton=NULL;
	}

	mMesh=NULL;

	BaseComponent::destroy();
}

void MeshComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
		mParent->visibleRemoved(this);

		if(mSkeleton!=NULL){
			mParent->remove(mSkeleton);
		}
	}

	BaseComponent::parentChanged(node);
	
	if(mParent!=NULL){
		mParent->spacialAttached(this);
		mParent->visibleAttached(this);

		if(mSkeleton!=NULL){
			mParent->attach(mSkeleton);
		}
	}
}

void MeshComponent::setMesh(const String &name){
	setMesh(shared_static_cast<Mesh>(mEngine->getMeshManager()->find(name)));
}

void MeshComponent::setMesh(Mesh *mesh){
	mSubMeshes.clear();

	if(mDynamicVertexData!=NULL){
		mDynamicVertexData->destroy();
		mDynamicVertexData=NULL;
	}

	if(mSkeleton!=NULL){
		mSkeleton->destroy();
		mSkeleton=NULL;
	}

	mMesh=mesh;

	if(mMesh==NULL){
		return;
	}

	setBound(mMesh->getBound());
 	setTransform(mMesh->getTransform());

	if(mMesh->getSkeleton()!=NULL){
		mSkeleton=new SkeletonComponent(mEngine,mMesh->getSkeleton());
		if(mParent!=NULL){
			mParent->attach(mSkeleton);
		}
		createVertexBuffer();
	}

	mSubMeshes.resize(mMesh->getNumSubMeshes());
	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh::ptr subMesh=new SubMesh(this,mMesh->getSubMesh(i));
		mSubMeshes[i]=subMesh;
		if(subMesh->mVertexData==NULL){
			if(mDynamicVertexData!=NULL){
				subMesh->mVertexData=mDynamicVertexData;
			}
			else{
				subMesh->mVertexData=mMesh->getStaticVertexData();
			}
		}

		if(subMesh->mMeshSubMesh->hasOwnTransform){
			subMesh->mTransform=new Transform(subMesh->mMeshSubMesh->transform);
		}

		if(mSharedRenderState!=NULL){
			subMesh->mMaterial=mEngine->getMaterialManager()->createSharedMaterial(subMesh->mMaterial,mSharedRenderState);
		}
	}
}

MeshComponent::SubMesh *MeshComponent::getSubMesh(const String &name){
	if(mMesh==NULL){
		return NULL;
	}

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		if(mSubMeshes[i]->mMeshSubMesh->getName().equals(name)){
			return mSubMeshes[i];
		}
	}

	return NULL;
}

void MeshComponent::setSkeleton(SkeletonComponent *skeleton){
	mSkeleton=skeleton;
}

void MeshComponent::frameUpdate(int dt,int scope){
	if(mSkeleton!=NULL){
		int lastUpdateFrame=mSkeleton->getLastUpdateFrame();
		if(lastUpdateFrame==-1 || lastUpdateFrame>=mParent->getScene()->getFrame()-1 || mParent->getScene()->getResetFrame()){
			updateVertexBuffer();
		}
	}
}

void MeshComponent::setBound(Bound::ptr bound){
	if(bound==NULL){
		mBound->reset();
	}
	else{
		mBound->set(bound);
	}
}

void MeshComponent::setTransform(Transform::ptr transform){
	if(transform==NULL){
		mTransform->reset();
	}
	else{
		mTransform->set(transform);
	}

	if(mSkeleton!=NULL){
		mSkeleton->setTransform(transform);
	}
}

void MeshComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	mWorldTransform->setTransform(mParent->getWorldTransform(),mTransform);
	mWorldBound->transform(mBound,mWorldTransform);

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh *subMesh=mSubMeshes[i];
		if(subMesh->mTransform){
			subMesh->mWorldTransform->setTransform(mWorldTransform,subMesh->mTransform);
			subMesh->mWorldBound->transform(subMesh->mMeshSubMesh->bound,subMesh->mWorldTransform);
		}
	}

	if(transform==mTransform){
		mParent->boundChanged();
	}
}

void MeshComponent::setSharedRenderState(RenderState::ptr renderState){
	int i;
	if(renderState==NULL){
		renderState=mEngine->getMaterialManager()->createRenderState();
		for(i=0;i<mSubMeshes.size();++i){
			if(i==0){
				mEngine->getMaterialManager()->modifyRenderState(renderState,mSubMeshes[i]->mMeshSubMesh->material->getRenderState());
			}
		}
	}

	mSharedRenderState=renderState;
	for(i=0;i<mSubMeshes.size();++i){
		mSubMeshes[i]->mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mSubMeshes[i]->mMeshSubMesh->material,mSharedRenderState);
	}
}

void MeshComponent::gatherRenderables(Camera *camera,RenderableSet *set){
	if(mRendered==false){
		return;
	}

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh *subMesh=mSubMeshes[i];
		if((subMesh->mScope&camera->getScope())!=0 && (subMesh->mTransform==NULL || camera->culled(subMesh->getRenderBound())==false)){
			set->queueRenderable(subMesh);
		}
	}

	if(mSkeleton!=NULL && mSkeleton->getRenderMaterial()!=NULL){
		set->queueRenderable(mSkeleton);
	}
}

void MeshComponent::createVertexBuffer(){
	if(mDynamicVertexData==NULL){
		if(mSkeleton!=NULL){
			VertexBuffer::ptr srcVertexBuffer=mMesh->getStaticVertexData()->getVertexBuffer(0);
			VertexFormat::ptr vertexFormat=srcVertexBuffer->getVertexFormat();
			int numVertexes=srcVertexBuffer->getSize();
			VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,vertexFormat,numVertexes);

			if(srcVertexBuffer!=NULL && vertexBuffer!=NULL){
				uint8 *srcData=srcVertexBuffer->lock(Buffer::Access_BIT_READ);
				uint8 *dstData=vertexBuffer->lock(Buffer::Access_BIT_WRITE);
				memcpy(dstData,srcData,srcVertexBuffer->getDataSize());
				vertexBuffer->unlock();
				srcVertexBuffer->unlock();

				mDynamicVertexData=new VertexData(vertexBuffer);
			}
		}
	}
}

void MeshComponent::updateVertexBuffer(){
	if(mDynamicVertexData!=NULL){
		VertexBuffer::ptr srcVertexBuffer=mMesh->getStaticVertexData()->getVertexBuffer(0);
		VertexBuffer::ptr dstVertexBuffer=mDynamicVertexData->getVertexBuffer(0);

		VertexFormat *format=srcVertexBuffer->getVertexFormat();
		int positionIndex=format->findElement(VertexFormat::Semantic_POSITION);
		int normalIndex=format->findElement(VertexFormat::Semantic_NORMAL);

		const Collection<Mesh::VertexBoneAssignmentList> &vbas=mMesh->getVertexBoneAssignments();

		{
			VertexBufferAccessor svba(srcVertexBuffer,Buffer::Access_BIT_READ);
			VertexBufferAccessor dvba(dstVertexBuffer,Buffer::Access_BIT_WRITE);

			// Recopy all data, so we get texcoords and color information copied over correctly
			memcpy(dvba.getData(),svba.getData(),srcVertexBuffer->getDataSize());

			Vector3 positionInitial,positionResult,normalInitial,normalResult,temp;
			int i,j;
			if(normalIndex>=0){
				for(i=0;i<srcVertexBuffer->getSize();++i){
					svba.get3(i,positionIndex,positionInitial);
					svba.get3(i,normalIndex,normalInitial);

					positionResult.reset();normalResult.reset();
					const Mesh::VertexBoneAssignmentList &vba=vbas[i];
					for(j=0;j<vba.size();++j){
						SkeletonComponent::Bone *bone=mSkeleton->getBone(vba[j].bone);

						Math::mul(temp,bone->boneSpaceRotate,positionInitial);
						Math::add(temp,bone->boneSpaceTranslate);
						Math::mul(temp,vba[j].weight);
						Math::add(positionResult,temp);

						Math::mul(temp,bone->boneSpaceRotate,normalInitial);
						Math::mul(temp,vba[j].weight);
						Math::add(normalResult,temp);
					}
					if(j==0){
						positionResult.set(positionInitial);
						normalResult.set(normalInitial);
					}

					dvba.set3(i,positionIndex,positionResult);
					dvba.set3(i,normalIndex,normalResult);
				}
			}
			else{
				for(i=0;i<srcVertexBuffer->getSize();++i){
					svba.get3(i,positionIndex,positionInitial);

					positionResult.reset();
					const Mesh::VertexBoneAssignmentList &vba=vbas[i];
					for(j=0;j<vba.size();++j){
						SkeletonComponent::Bone *bone=mSkeleton->getBone(vba[j].bone);

						Math::mul(temp,bone->boneSpaceRotate,positionInitial);
						Math::add(temp,bone->boneSpaceTranslate);
						Math::mul(temp,vba[j].weight);
						Math::add(positionResult,temp);
					}
					if(j==0){
						positionResult.set(positionInitial);
					}

					dvba.set3(i,positionIndex,positionResult);
				}
			}

			svba.unlock();
			dvba.unlock();
		}
	}
}

}
}
