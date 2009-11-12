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
#include <toadlet/tadpole/entity/MeshEntity.h>
#include <toadlet/tadpole/entity/ParentEntity.h>
#include <toadlet/tadpole/animation/SkeletonAnimation.h>
#include <toadlet/tadpole/Engine.h>
#include <stdlib.h>
#if defined(TOADLET_PLATFORM_POSIX)
	#include <string.h>
#endif

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::animation;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{
namespace entity{

TOADLET_ENTITY_IMPLEMENT(MeshEntity,"toadlet::tadpole::entity::MeshEntity");

MeshEntity::SubMesh::SubMesh(MeshEntity *meshEntity,Mesh::SubMesh *meshSubMesh){
	this->meshEntity=meshEntity;
	this->meshSubMesh=meshSubMesh;
}

void MeshEntity::SubMesh::render(Renderer *renderer) const{
	renderer->renderPrimitive(vertexData,indexData);
}

MeshEntity::MeshAnimationController::MeshAnimationController(MeshEntity *entity):AnimationController(),
	mMeshEntity(NULL),
	//mAnimation,
	mStartingFrame(0)
{
	mMeshEntity=entity;
	mAnimation=SkeletonAnimation::ptr(new SkeletonAnimation());
	mAnimation->setTarget(mMeshEntity->getSkeleton());
	attachAnimation(mAnimation);
}

void MeshEntity::MeshAnimationController::setSequenceIndex(int index){
	mAnimation->setSequenceIndex(index);
}

int MeshEntity::MeshAnimationController::getSequenceIndex() const{
	return mAnimation->getSequenceIndex();
}

void MeshEntity::MeshAnimationController::start(){
	if(isRunning()){
		stop();
	}

	mMeshEntity->setReceiveUpdates(true);

	AnimationController::start();
	if(mMeshEntity->getEngine()->getScene()!=NULL){
		mStartingFrame=mMeshEntity->getEngine()->getScene()->getLogicFrame();
	}
}

void MeshEntity::MeshAnimationController::stop(){
	if(isRunning()==false){
		return;
	}

	mMeshEntity->setReceiveUpdates(false);

	AnimationController::stop();
}	

void MeshEntity::MeshAnimationController::logicUpdate(int dt){
	if(mMeshEntity->getEngine()->getScene()==NULL || mStartingFrame!=mMeshEntity->getEngine()->getScene()->getLogicFrame()){
		AnimationController::logicUpdate(dt);
	}
}

void MeshEntity::MeshAnimationController::visualUpdate(int dt){
	AnimationController::visualUpdate(dt);
}

void MeshEntity::MeshAnimationController::skeletonChanged(){
	mAnimation->setTarget(mMeshEntity->getSkeleton());
}

MeshEntity::MeshEntity():RenderableEntity()
	//mMesh,
	//mSubMeshes,
	//mSkeleton,
	//mDynamicVertexData,

	//mAnimationController
{
}

Entity *MeshEntity::create(Engine *engine){
	super::create(engine);

	mMesh=NULL;
	mSubMeshes.clear();
	mSkeleton=NULL;
	mDynamicVertexData=NULL;

	mAnimationController=NULL;

	return this;
}

void MeshEntity::destroy(){
	mSubMeshes.clear();

	if(mAnimationController!=NULL){
		mAnimationController->stop();
		mAnimationController=NULL;
	}

	if(mDynamicVertexData!=NULL){
		mDynamicVertexData->destroy();
		mDynamicVertexData=NULL;
	}

	super::destroy();
}

void MeshEntity::load(const String &name){
	load(mEngine->getMeshManager()->findMesh(name));
}

void MeshEntity::load(Mesh::ptr mesh){
	mSubMeshes.clear();

	if(mMesh!=NULL){
		mMesh->release();
	}

	mMesh=mesh;

	if(mMesh==NULL){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"Invalid Mesh");
		return;
	}
	else{
		mMesh->retain();
	}

	if(mMesh->boundingRadius!=Math::ONE){
		mBoundingRadius=Math::mul(mMesh->worldScale,mMesh->boundingRadius);
	}
	else{
		mBoundingRadius=-Math::ONE;
	}

	if(mMesh->skeleton!=NULL){
		mSkeleton=MeshEntitySkeleton::ptr(new MeshEntitySkeleton(this,mMesh->skeleton));

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

MeshEntity::SubMesh *MeshEntity::getSubMesh(const String &name){
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

void MeshEntity::setSkeleton(MeshEntitySkeleton::ptr skeleton){
	mSkeleton=skeleton;

	if(mAnimationController!=NULL){
		mAnimationController->skeletonChanged();
	}
}

MeshEntity::MeshAnimationController::ptr MeshEntity::getAnimationController(){
	if(mAnimationController==NULL){
		mAnimationController=MeshAnimationController::ptr(new MeshAnimationController(this));
	}

	return mAnimationController;
}

void MeshEntity::logicUpdate(int dt){
	if(mAnimationController!=NULL){
		mAnimationController->logicUpdate(dt);
	}
}

void MeshEntity::visualUpdate(int dt){
	if(mAnimationController!=NULL){
		mAnimationController->visualUpdate(dt);
	}
}

void MeshEntity::queueRenderables(Scene *scene){
	if(mMesh!=NULL && mMesh->worldScale!=Math::ONE){
		mVisualWorldTransform.reset();
		mVisualWorldTransform.setAt(0,0,mMesh->worldScale);
		mVisualWorldTransform.setAt(1,1,mMesh->worldScale);
		mVisualWorldTransform.setAt(2,2,mMesh->worldScale);
		Math::preMul(mVisualWorldTransform,mVisualTransform);
		Math::preMul(mVisualWorldTransform,mParent->getVisualWorldTransform());
	}

	if(mSkeleton!=NULL){
		int lastVisualUpdateFrame=mSkeleton->getLastVisualUpdateFrame();
		if(lastVisualUpdateFrame==-1 || lastVisualUpdateFrame==scene->getVisualFrame()){
			updateVertexBuffer();
		}
	}

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		scene->queueRenderable(mSubMeshes[i]);
	}

	if(mSkeleton!=NULL && mSkeleton->getRenderMaterial()!=NULL){
		scene->queueRenderable(mSkeleton);
	}
}

void MeshEntity::createVertexBuffer(){
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

void MeshEntity::updateVertexBuffer(){
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
			// TODO: Research this more, and see if this is a known problem
			memcpy(dvba.getData(),svba.getData(),srcVertexBuffer->getDataSize());

			Vector3 &vector=cache_updateVertexBuffer_vector.reset();

			int i;
			if(format->hasVertexElementOfType(VertexElement::Type_NORMAL)){
				int normalElement=format->getVertexElementIndexOfType(VertexElement::Type_NORMAL);
				for(i=0;i<srcVertexBuffer->getSize();++i){
					MeshEntitySkeleton::Bone *bone=mSkeleton->getBone(mMesh->vertexBoneAssignments[i][0].bone); // TODO: Implement all bones

					svba.get3(i,positionElement,vector);

					Math::mul(vector,bone->boneSpaceRotate);
					Math::add(vector,bone->boneSpaceTranslate);

					dvba.set3(i,positionElement,vector);

					svba.get3(i,normalElement,vector);

					Math::mul(vector,bone->boneSpaceRotate);

					dvba.set3(i,normalElement,vector);
				}
			}
			else{
				for(i=0;i<srcVertexBuffer->getSize();++i){
					MeshEntitySkeleton::Bone *bone=mSkeleton->getBone(mMesh->vertexBoneAssignments[i][0].bone); // TODO: Implement all bones

					svba.get3(i,positionElement,vector);

					Math::mul(vector,bone->boneSpaceRotate);
					Math::add(vector,bone->boneSpaceTranslate);

					dvba.set3(i,positionElement,vector);
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
