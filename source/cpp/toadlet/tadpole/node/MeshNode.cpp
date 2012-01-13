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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/animation/SkeletonAnimation.h>

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(MeshNode,Categories::TOADLET_TADPOLE_NODE+".MeshNode");

MeshNode::SubMesh::SubMesh(MeshNode *meshNode,Mesh::SubMesh *meshSubMesh):
	hasOwnTransform(false),
	scope(-1)
{
	this->meshNode=meshNode;
	this->meshSubMesh=meshSubMesh;
	this->indexData=meshSubMesh->indexData;
	this->vertexData=meshSubMesh->vertexData;
	this->material=meshSubMesh->material;
	if(material!=NULL){
		material->retain();
	}
}

void MeshNode::SubMesh::render(SceneRenderer *renderer) const{
	renderer->getDevice()->renderPrimitive(vertexData,indexData);
}

MeshNode::MeshController::MeshController(MeshNode *node):Controller(),
	mMeshNode(NULL)
	//mAnimation
{
	mMeshNode=node;
	mAnimation=SkeletonAnimation::ptr(new SkeletonAnimation());
	mAnimation->setTarget(mMeshNode->getSkeleton());
	attach(mAnimation);
}

void MeshNode::MeshController::skeletonChanged(){
	mAnimation->setTarget(mMeshNode->getSkeleton());
}

MeshNode::MeshNode():super(),
	mRendered(false)
	//mMesh,
	//mSubMeshes,
	//mSkeleton,
	//mDynamicVertexData,

	//mController
{}

Node *MeshNode::create(Scene *scene){
	super::create(scene);
	setCameraAligned(false);

	mRendered=true;

	return this;
}

void MeshNode::destroy(){
	int i;
	for(i=0;i<mSubMeshes.size();++i){
		mSubMeshes[i]->destroy();
	}
	mSubMeshes.clear();

	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	if(mController!=NULL){
		mController->stop();
		removeController(mController);
		mController=NULL;
	}

	if(mDynamicVertexData!=NULL){
		mDynamicVertexData->destroy();
		mDynamicVertexData=NULL;
	}

	if(mMesh!=NULL){
		mMesh->release();
		mMesh=NULL;
	}

	if(mSkeleton!=NULL){
		mSkeleton->destroy();
		mSkeleton=NULL;
	}

	super::destroy();
}

Node *MeshNode::set(Node *node){
	super::set(node);

	MeshNode *meshNode=(MeshNode*)node;
	setMesh(meshNode->getMesh());

	return this;
}

void *MeshNode::hasInterface(int type){
	switch(type){
		case InterfaceType_VISIBLE:
			return (Visible*)this;
		default:
			return NULL;
	}
}

void MeshNode::setMesh(const String &name){
	setMesh(mEngine->getMeshManager()->findMesh(name));
}

void MeshNode::setMesh(Mesh::ptr mesh){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"MeshNode::setMesh");

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		mSubMeshes[i]->destroy();
	}
	mSubMeshes.clear();

	if(mController!=NULL){
		mController->stop();
		removeController(mController);
		mController=NULL;
	}

	if(mDynamicVertexData!=NULL){
		mDynamicVertexData->destroy();
		mDynamicVertexData=NULL;
	}

	if(mMesh!=NULL){
		mMesh->release();
		mMesh=NULL;
	}

	if(mSkeleton!=NULL){
		mSkeleton->destroy();
		mSkeleton=NULL;
	}

	if(mesh==NULL){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"invalid Mesh");
		return;
	}

	mMesh=mesh;
	mMesh->retain();

	setTransform(mMesh->getTransform());
	setBound(mMesh->getBound());

	if(mMesh->getSkeleton()!=NULL){
		mSkeleton=MeshNodeSkeleton::ptr(new MeshNodeSkeleton(this,mMesh->getSkeleton()));
		createVertexBuffer();
	}

	mSubMeshes.resize(mMesh->getNumSubMeshes());
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh::ptr subMesh(new SubMesh(this,mMesh->getSubMesh(i)));
		mSubMeshes[i]=subMesh;
		if(subMesh->vertexData==NULL){
			if(mDynamicVertexData!=NULL){
				subMesh->vertexData=mDynamicVertexData;
			}
			else{
				subMesh->vertexData=mMesh->getStaticVertexData();
			}
		}

		subMesh->hasOwnTransform=subMesh->meshSubMesh->hasOwnTransform;

		if(mSharedRenderState!=NULL){
			Material::ptr material=mEngine->getMaterialManager()->createSharedMaterial(subMesh->material,mSharedRenderState);
			subMesh->material->release();
			subMesh->material=material;
		}
	}
}

MeshNode::SubMesh *MeshNode::getSubMesh(const String &name){
	if(mMesh==NULL){
		return NULL;
	}

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		if(mSubMeshes[i]->meshSubMesh->name.equals(name)){
			return mSubMeshes[i];
		}
	}

	return NULL;
}

void MeshNode::setSkeleton(MeshNodeSkeleton::ptr skeleton){
	mSkeleton=skeleton;

	if(mController!=NULL){
		mController->skeletonChanged();
	}
}

MeshNode::MeshController::ptr MeshNode::getController(){
	if(mController==NULL){
		mController=MeshController::ptr(new MeshController(this));
		addController(mController);
	}

	return mController;
}

void MeshNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	if(mSkeleton!=NULL){
		int lastUpdateFrame=mSkeleton->getLastUpdateFrame();
		if(lastUpdateFrame==-1 || lastUpdateFrame==mScene->getFrame() || mScene->getResetFrame()){
			updateVertexBuffer();
		}
	}
}

void MeshNode::updateWorldTransform(){
	if(mSkeleton!=NULL){
		mBound.set(mSkeleton->getBound());
	}

	super::updateWorldTransform();

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh *subMesh=mSubMeshes[i];
		if(subMesh->hasOwnTransform){
			subMesh->worldTransform.setTransform(mWorldTransform,subMesh->meshSubMesh->transform);
			subMesh->worldBound.transform(subMesh->meshSubMesh->bound,subMesh->worldTransform);
		}
	}
}

RenderState::ptr MeshNode::getSharedRenderState(){
	if(mSharedRenderState==NULL){
		mSharedRenderState=mEngine->getMaterialManager()->createRenderState();
		int i;
		for(i=0;i<mSubMeshes.size();++i){
			Material::ptr material=mEngine->getMaterialManager()->createSharedMaterial(mSubMeshes[i]->material,mSharedRenderState);
			mSubMeshes[i]->material->release();
			mSubMeshes[i]->material=material;
		}
	}

	return mSharedRenderState;
}

void MeshNode::gatherRenderables(CameraNode *camera,RenderableSet *set){
	super::gatherRenderables(camera,set);

	if(mRendered==false){
		return;
	}

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh *subMesh=mSubMeshes[i];
		if((subMesh->scope&camera->getScope())!=0 && (subMesh->hasOwnTransform==false || camera->culled(subMesh->worldBound)==false)){
			set->queueRenderable(subMesh);
		}
	}

	if(mSkeleton!=NULL && mSkeleton->getRenderMaterial()!=NULL){
		set->queueRenderable(mSkeleton);
	}
}

void MeshNode::createVertexBuffer(){
	if(mDynamicVertexData==NULL){
		if(mSkeleton!=NULL){
			VertexBuffer::ptr srcVertexBuffer=mMesh->getStaticVertexData()->getVertexBuffer(0);
			VertexFormat::ptr vertexFormat=srcVertexBuffer->getVertexFormat();
			int numVertexes=srcVertexBuffer->getSize();
			VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,vertexFormat,numVertexes);

			uint8 *srcData=srcVertexBuffer->lock(Buffer::Access_BIT_READ);
			uint8 *dstData=vertexBuffer->lock(Buffer::Access_BIT_WRITE);
			memcpy(dstData,srcData,srcVertexBuffer->getDataSize());
			vertexBuffer->unlock();
			srcVertexBuffer->unlock();

			mDynamicVertexData=VertexData::ptr(new VertexData(vertexBuffer));
		}
	}
}

void MeshNode::updateVertexBuffer(){
	if(mDynamicVertexData!=NULL){
		VertexBuffer::ptr srcVertexBuffer=mMesh->getStaticVertexData()->getVertexBuffer(0);
		VertexBuffer::ptr dstVertexBuffer=mDynamicVertexData->getVertexBuffer(0);

		VertexFormat *format=srcVertexBuffer->getVertexFormat();
		int positionIndex=format->findElement(VertexFormat::Semantic_POSITION);
		int normalIndex=format->findElement(VertexFormat::Semantic_NORMAL);

		const Collection<Mesh::VertexBoneAssignmentList> &vbas=mMesh->getVertexBoneAssignments();

		{
			svba.lock(srcVertexBuffer,Buffer::Access_BIT_READ);
			dvba.lock(dstVertexBuffer,Buffer::Access_BIT_WRITE);

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
						MeshNodeSkeleton::Bone *bone=mSkeleton->getBone(vba[j].bone);

						/// @todo: Move these ifs out to separate loops
						if(bone->useMatrixTransforms){
							Math::mulPoint3Full(temp,bone->boneSpaceMatrix,positionInitial);
						}
						else{
							Math::mul(temp,bone->boneSpaceRotate,positionInitial);
							Math::add(temp,bone->boneSpaceTranslate);
						}
						Math::mul(temp,vba[j].weight);
						Math::add(positionResult,temp);

						/// @todo: Move these ifs out to separate loops
						if(bone->useMatrixTransforms){
							Math::mul(temp,bone->boneSpaceMatrix,normalInitial);
						}
						else{
							Math::mul(temp,bone->boneSpaceRotate,normalInitial);
						}
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
						MeshNodeSkeleton::Bone *bone=mSkeleton->getBone(vba[j].bone);

						if(bone->useMatrixTransforms){
							Math::mulPoint3Full(temp,bone->boneSpaceMatrix,positionInitial);
						}
						else{
							Math::mul(temp,bone->boneSpaceRotate,positionInitial);
							Math::add(temp,bone->boneSpaceTranslate);
						}
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
}
