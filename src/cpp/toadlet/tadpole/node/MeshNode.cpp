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
#include <stdlib.h>
#include <string.h> // memcpy

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(MeshNode,Categories::TOADLET_TADPOLE_NODE+".MeshNode");

MeshNode::SubMesh::SubMesh(MeshNode *meshNode,Mesh::SubMesh *meshSubMesh){
	this->meshNode=meshNode;
	this->meshSubMesh=meshSubMesh;
}

void MeshNode::SubMesh::render(Renderer *renderer) const{
	renderer->renderPrimitive(vertexData,indexData);
}

MeshNode::MeshAnimationController::MeshAnimationController(MeshNode *node):AnimationController(),
	mMeshNode(NULL),
	//mAnimation,
	mStartingFrame(0)
{
	mMeshNode=node;
	mAnimation=SkeletonAnimation::ptr(new SkeletonAnimation());
	mAnimation->setTarget(mMeshNode->getSkeleton());
	attach(mAnimation);
}

void MeshNode::MeshAnimationController::setSequenceIndex(int index){
	mAnimation->setSequenceIndex(index);
}

int MeshNode::MeshAnimationController::getSequenceIndex() const{
	return mAnimation->getSequenceIndex();
}

void MeshNode::MeshAnimationController::start(){
	if(isRunning()){
		stop();
	}

	/// @todo: hacky hack
	mMeshNode->mDeactivateCount=-1;

	AnimationController::start();
	if(mMeshNode->getScene()!=NULL){
		mStartingFrame=mMeshNode->getScene()->getLogicFrame();
	}
}

void MeshNode::MeshAnimationController::stop(){
	if(isRunning()==false){
		return;
	}

	/// @todo: hacky hack
	mMeshNode->mDeactivateCount=0;

	AnimationController::stop();
}	

void MeshNode::MeshAnimationController::update(int dt){
	if(mStartingFrame!=mMeshNode->getScene()->getLogicFrame()){
		AnimationController::update(dt);
	}
}

void MeshNode::MeshAnimationController::skeletonChanged(){
	mAnimation->setTarget(mMeshNode->getSkeleton());
}

MeshNode::MeshNode():super()
	//mMesh,
	//mSubMeshes,
	//mSkeleton,
	//mDynamicVertexData,

	//mAnimationController
{
}

Node *MeshNode::create(Scene *scene){
	super::create(scene);
	setCameraAligned(false);

	mMesh=NULL;
	mSubMeshes.clear();
	mSkeleton=NULL;
	mDynamicVertexData=NULL;

	mAnimationController=NULL;

	return this;
}

void MeshNode::destroy(){
	mSubMeshes.clear();

	if(mAnimationController!=NULL){
		mAnimationController->stop();
		mAnimationController=NULL;
	}

	if(mDynamicVertexData!=NULL){
		mDynamicVertexData->destroy();
		mDynamicVertexData=NULL;
	}

	if(mMesh!=NULL){
		mMesh->release();
		mMesh=NULL;
	}

	super::destroy();
}

void MeshNode::setMesh(const String &name){
	setMesh(mEngine->getMeshManager()->findMesh(name));
}

void MeshNode::setMesh(Mesh::ptr mesh){
	mSubMeshes.clear();

	if(mAnimationController!=NULL){
		mAnimationController->stop();
		mAnimationController=NULL;
	}

	if(mDynamicVertexData!=NULL){
		mDynamicVertexData->destroy();
		mDynamicVertexData=NULL;
	}

	if(mMesh!=NULL){
		mMesh->release();
		mMesh=NULL;
	}

	if(mesh==NULL){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"invalid Mesh");
		return;
	}

	mMesh=mesh;
	mMesh->retain();

	setScale(mMesh->worldScale);

	mLocalBound.origin.set(mMesh->bound.origin);
	mLocalBound.radius=Math::mul(mMesh->bound.radius,mMesh->worldScale);

	if(mMesh->skeleton!=NULL){
		mSkeleton=MeshNodeSkeleton::ptr(new MeshNodeSkeleton(this,mMesh->skeleton));

		createVertexBuffer();
	}

	mSubMeshes.resize(mMesh->subMeshes.size());
	int i;
	for(i=0;i<mMesh->subMeshes.size();++i){
		SubMesh::ptr subMesh(new SubMesh(this,mMesh->subMeshes[i]));
		mSubMeshes[i]=subMesh;
		subMesh->material=mMesh->subMeshes[i]->material;
		subMesh->indexData=mMesh->subMeshes[i]->indexData;
		if(mDynamicVertexData!=NULL){
			subMesh->vertexData=mDynamicVertexData;
		}
		else{
			subMesh->vertexData=mMesh->staticVertexData;
		}
	}
}

MeshNode::SubMesh *MeshNode::getSubMesh(const String &name){
	if(mMesh==NULL){
		return NULL;
	}

	int i;
	for(i=0;i<mMesh->subMeshes.size();++i){
		if(mMesh->subMeshes[i]->name.equals(name)){
			return mSubMeshes[i];
		}
	}

	return NULL;
}

void MeshNode::setSkeleton(MeshNodeSkeleton::ptr skeleton){
	mSkeleton=skeleton;

	if(mAnimationController!=NULL){
		mAnimationController->skeletonChanged();
	}
}

MeshNode::MeshAnimationController::ptr MeshNode::getAnimationController(){
	if(mAnimationController==NULL){
		mAnimationController=MeshAnimationController::ptr(new MeshAnimationController(this));
	}

	return mAnimationController;
}

void MeshNode::frameUpdate(int dt){
	super::frameUpdate(dt);

	if(mSkeleton!=NULL){
		int lastUpdateFrame=mSkeleton->getLastUpdateFrame();
		if(lastUpdateFrame==-1 || lastUpdateFrame==mScene->getFrame()){
			updateVertexBuffer();
		}
	}
}

void MeshNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		queue->queueRenderable(mSubMeshes[i]);
	}

	if(mSkeleton!=NULL && mSkeleton->getRenderMaterial()!=NULL){
		queue->queueRenderable(mSkeleton);
	}
}

void MeshNode::createVertexBuffer(){
	if(mDynamicVertexData==NULL){
		if(mSkeleton!=NULL){
			VertexBuffer::ptr srcVertexBuffer=mMesh->staticVertexData->getVertexBuffer(0);
			VertexFormat::ptr vertexFormat=srcVertexBuffer->getVertexFormat();
			int numVertexes=srcVertexBuffer->getSize();
			VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_DYNAMIC,Buffer::AccessType_WRITE_ONLY,vertexFormat,numVertexes);

			uint8 *srcData=srcVertexBuffer->lock(Buffer::AccessType_READ_ONLY);
			uint8 *dstData=vertexBuffer->lock(Buffer::AccessType_WRITE_ONLY);
			memcpy(dstData,srcData,srcVertexBuffer->getDataSize());
			vertexBuffer->unlock();
			srcVertexBuffer->unlock();

			mDynamicVertexData=VertexData::ptr(new VertexData(vertexBuffer));
		}
	}
}

void MeshNode::updateVertexBuffer(){
	if(mDynamicVertexData!=NULL){
		VertexBuffer::ptr srcVertexBuffer=mMesh->staticVertexData->getVertexBuffer(0);
		VertexBuffer::ptr dstVertexBuffer=mDynamicVertexData->getVertexBuffer(0);

		VertexFormat *format=srcVertexBuffer->getVertexFormat();
		int positionElement=format->getVertexElementIndexOfType(VertexElement::Type_POSITION);

		{
			svba.lock(srcVertexBuffer,Buffer::AccessType_READ_ONLY);
			dvba.lock(dstVertexBuffer,Buffer::AccessType_WRITE_ONLY);

			// It appears that hardware buffers in opengl don't store their previous data if you want to write to them.
			// Basically, if you want to write any of it, you have to write it all.
			// So we re-copy over all the data to start
			// NOTE: Probably due to UT_WRITE_ONLY on creation, makes it mean if we lock it, we will rewrite all that data
			memcpy(dvba.getData(),svba.getData(),srcVertexBuffer->getDataSize());

			Vector3 &positionVector=cache_updateVertexBuffer_positionVector.reset();
			Vector3 &normalVector=cache_updateVertexBuffer_normalVector.reset();

			int i;
			if(format->hasVertexElementOfType(VertexElement::Type_NORMAL)){
				int normalElement=format->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
				for(i=0;i<srcVertexBuffer->getSize();++i){
					svba.get3(i,positionElement,positionVector);
					svba.get3(i,normalElement,normalVector);

					const Mesh::VertexBoneAssignmentList &vba=mMesh->vertexBoneAssignments[i];
					if(vba.size()>0){
						MeshNodeSkeleton::Bone *bone=mSkeleton->getBone(mMesh->vertexBoneAssignments[i][0].bone); /// @todo: Implement all bones

						Math::mul(positionVector,bone->boneSpaceRotate);
						Math::add(positionVector,bone->boneSpaceTranslate);

						Math::mul(normalVector,bone->boneSpaceRotate);
					}
				
					dvba.set3(i,positionElement,positionVector);
					dvba.set3(i,normalElement,normalVector);
				}
			}
			else{
				for(i=0;i<srcVertexBuffer->getSize();++i){
					svba.get3(i,positionElement,positionVector);

					const Mesh::VertexBoneAssignmentList &vba=mMesh->vertexBoneAssignments[i];
					if(vba.size()>0){
						MeshNodeSkeleton::Bone *bone=mSkeleton->getBone(mMesh->vertexBoneAssignments[i][0].bone); /// @todo: Implement all bones

						Math::mul(positionVector,bone->boneSpaceRotate);
						Math::add(positionVector,bone->boneSpaceTranslate);
					}

					dvba.set3(i,positionElement,positionVector);
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
