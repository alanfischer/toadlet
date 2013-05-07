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

#include <toadlet/tadpole/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/SkeletonComponent.h>

namespace toadlet{
namespace tadpole{

SkeletonComponent::SequenceAnimation::SequenceAnimation(Sequence *sequence):
	mSequence(sequence),
	mValue(0),
	mWeight(0),
	mScope(-1)
{
}

SkeletonComponent::SkeletonComponent(Engine *engine,Skeleton *skeleton):
	mEngine(NULL),
	//mSkeleton,
	//mBones,
	mLastUpdateFrame(-1)
{
	mEngine=engine;
	mSkeleton=skeleton;

	mBones.resize(skeleton->getNumBones());
	int i;
	for(i=0;i<skeleton->getNumBones();++i){
		mBones[i]=Bone::ptr(new Bone(i));
	}

	mAnimations.resize(mSkeleton->getNumSequences());
	for(i=0;i<mSkeleton->getNumSequences();++i){
		mAnimations[i]=new SequenceAnimation(mSkeleton->getSequence(i));
		mAnimations[i]->mTrackHints.resize(mBones.size());
	}

	mTransform=new Transform();
	mTransform->setTransformListener(this);
	mBound=new Bound();
	mWorldTransform=new Transform();
	mWorldBound=new Bound();
	
	updateBones();
}

void SkeletonComponent::destroy(){
	mSkeletonMaterial=NULL;

	destroySkeletonBuffers();

	BaseComponent::destroy();
}

void SkeletonComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
		mParent->animatableRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->spacialAttached(this);
		mParent->animatableAttached(this);
	}
}

void SkeletonComponent::frameUpdate(int dt,int scope){
	updateBones();
}

void SkeletonComponent::updateBones(){
	AABox bound;

	int i;
	for(i=0;i<mBones.size();++i){
		updateBone(mBones[i]);

		if(i==0){
			bound.set(mBones[i]->worldBound);
		}
		else{
			bound.merge(mBones[i]->worldBound);
		}
	}

	mBound->set(bound);
	
	if(mSkeletonMaterial!=NULL){
		updateSkeletonBuffers();
	}

	if(mParent!=NULL){
		mParent->activate();
		mLastUpdateFrame=mParent->getScene()->getFrame();
	}
	else{
		mLastUpdateFrame=-1;
	}
}

int SkeletonComponent::getBoneIndex(const String &name) const{
	int i;
	for(i=0;i<mSkeleton->getNumBones();++i){
		if(mSkeleton->getBone(i)->name.equals(name)){
			return i;
		}
	}

	return -1;
}

String SkeletonComponent::getBoneName(int index) const{
	if(index>=0 && index<mSkeleton->getNumBones()){
		return mSkeleton->getBone(index)->name;
	}
	else{
		return (char*)NULL;
	}
}

void SkeletonComponent::setBoneScope(int i,int scope,bool recurse){
	if(i==-1){
		for(i=0;i<mBones.size();++i){
			if(mSkeleton->getBone(i)->parentIndex==-1){
				setBoneScope(i,scope,recurse);
			}
		}
		return;
	}

	mBones[i]->scope=scope;

	if(recurse){
		int parent=i;
		for(i=0;i<mBones.size();++i){
			if(mSkeleton->getBone(i)->parentIndex==parent){
				setBoneScope(i,scope,recurse);
			}
		}
	}
}

void SkeletonComponent::setTransform(Transform::ptr transform){
	if(transform==NULL){
		mTransform->reset();
	}
	else{
		mTransform->set(transform);
	}
}

void SkeletonComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	mWorldTransform->setTransform(mParent->getWorldTransform(),mTransform);
	mWorldBound->transform(mBound,mWorldTransform);

	if(transform==mTransform){
		mParent->boundChanged();
	}
}

int SkeletonComponent::updateBoneTransformation(Bone *bone){
	/// @todo: Support blending weighted animations
	int flags=bone->dontUpdateFlags;
	int i;
	for(i=0;i<mAnimations.size();++i){
		SequenceAnimation *animation=mAnimations[i];
		Sequence *sequence=animation->mSequence;
		Track *track=sequence->getTrack(bone->index);
		scalar time=animation->mValue;
		if(track!=NULL && (animation->mScope&bone->scope)!=0 && animation->mWeight>0){
			int f1=-1,f2=-1;
			scalar t=track->getKeyFramesAtTime(time,f1,f2,animation->mTrackHints[bone->index]);
			if(f1==-1 || f2==-1){
				continue;
			}

			int positionIndex=track->getFormat()->findElement(VertexFormat::Semantic_POSITION);
			int rotateIndex=track->getFormat()->findElement(VertexFormat::Semantic_ROTATE);
			
			VertexBufferAccessor &vba=track->getAccessor();
			if(positionIndex>=0 && (bone->dontUpdateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
				Vector3 t1,t2;
				vba.get3(f1,positionIndex,t1);
				vba.get3(f2,positionIndex,t2);
				Math::lerp(bone->localTranslate,t1,t2,t);
			}
			if(rotateIndex>=0 && (bone->dontUpdateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
				Quaternion r1,r2;
				vba.get4(f1,rotateIndex,r1);
				vba.get4(f2,rotateIndex,r2);
				Math::lerp(bone->localRotate,r1,r2,t);
				Math::normalizeCarefully(bone->localRotate,0);
			}
			flags=BoneSpaceUpdate_FLAG_TRANSLATE|BoneSpaceUpdate_FLAG_ROTATE;
		}
	}

	return flags;
}

void SkeletonComponent::updateBone(Bone *bone){
	int updateFlags=BoneSpaceUpdate_NONE;

	Skeleton::Bone *sbone=mSkeleton->getBone(bone->index);

	if(bone->controller==NULL){
		updateFlags=updateBoneTransformation(bone);
	}
	else{
		updateFlags=bone->controller->updateBoneTransform(bone);
	}

	bone->useMatrixTransforms=((updateFlags&BoneSpaceUpdate_FLAG_MATRIX)!=0);

	Bone *parent=NULL;
	int parentIndex=sbone->parentIndex;
	if(parentIndex>=0){
		parent=mBones[parentIndex];

		// If a parent only uses matrix transforms, then this bone must do it also, since the quaternions are not available
		bone->useMatrixTransforms|=parent->useMatrixTransforms;
	}

	Vector3 vector;

	if((updateFlags&BoneSpaceUpdate_FLAG_WORLD)==0){
		if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
			bone->localRotate.set(sbone->rotate);
		}

		if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
			bone->localTranslate.set(sbone->translate);
		}

		if(bone->useMatrixTransforms){
			Math::setMatrix4x4FromQuaternion(bone->worldMatrix,bone->localRotate);
			Math::setMatrix4x4FromTranslate(bone->worldMatrix,bone->localTranslate);
			if(parent!=NULL){
				Math::preMul(bone->worldMatrix,parent->worldMatrix);
			}
		}
		else{
			if(parent==NULL){
				bone->worldRotate.set(bone->localRotate);
				bone->worldTranslate.set(bone->localTranslate);
			}
			else{
				Math::mul(bone->worldRotate,parent->worldRotate,bone->localRotate);
				Math::mul(vector,parent->worldRotate,bone->localTranslate);
				Math::add(bone->worldTranslate,parent->worldTranslate,vector);
			}
		}
	}
	else{
		if(bone->useMatrixTransforms){
			Math::setMatrix4x4FromQuaternion(bone->worldMatrix,sbone->rotate);
			Math::setMatrix4x4FromTranslate(bone->worldMatrix,sbone->translate);
		}
		else{
			if(parent==NULL){
				if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
					bone->worldRotate.set(sbone->rotate);
				}
				if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
					bone->worldTranslate.set(sbone->translate);
				}
			}
			else{
				if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
					Math::mul(bone->worldRotate,parent->worldRotate,sbone->rotate);
				}
				if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
					Math::mul(vector,parent->worldRotate,sbone->translate);
					Math::add(bone->worldTranslate,parent->worldTranslate,vector);
				}
			}
		}
	}

	if(bone->useMatrixTransforms){
		Math::setMatrix4x4FromQuaternion(bone->boneSpaceMatrix,sbone->worldToBoneRotate);
		Math::preMul(bone->boneSpaceMatrix,bone->worldMatrix);

		Math::mulPoint3Fast(vector,bone->worldMatrix,sbone->worldToBoneTranslate);
		Math::setMatrix4x4FromTranslate(bone->boneSpaceMatrix,vector);

		Math::mul(bone->worldBound,bone->worldMatrix,sbone->bound);
	}
	else{
		Math::mul(bone->boneSpaceRotate,bone->worldRotate,sbone->worldToBoneRotate);

		Math::mul(vector,bone->worldRotate,sbone->worldToBoneTranslate);
		Math::add(bone->boneSpaceTranslate,bone->worldTranslate,vector);

		Math::mul(bone->worldBound,bone->worldRotate,sbone->bound);
		Math::add(bone->worldBound,bone->worldTranslate);
	}
}

void SkeletonComponent::setRenderSkeleton(bool skeleton){
	if(skeleton){
		if(mSkeletonMaterial==NULL){
			RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
			if(renderState!=NULL){
				renderState->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
				renderState->setPointState(PointState(true,Math::fromInt(8),false,0,0,0,0,0));

				mSkeletonMaterial=mEngine->createDiffuseMaterial(NULL,renderState);
			}
		}
	}
	else{
		if(mSkeletonMaterial!=NULL){
			mSkeletonMaterial->destroy();
		}
	}

	// Always recreate the buffers, since we may have a different skeleton
	destroySkeletonBuffers();
	if(mSkeletonMaterial!=NULL){
		createSkeletonBuffers();
	}
}

void SkeletonComponent::createSkeletonBuffers(){
	int i;

	IndexBuffer::ptr skeletonIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,(mBones.size()-1)*2);
	IndexBufferAccessor iba;
	{
		iba.lock(skeletonIndexBuffer,Buffer::Access_BIT_WRITE);
		for(i=1;i<mBones.size();++i){
			iba.set((i-1)*2+0,mSkeleton->getBone(i)->parentIndex<0?0:mSkeleton->getBone(i)->parentIndex);
			iba.set((i-1)*2+1,i);
		}
		iba.unlock();
	}

	mSkeletonIndexData=new IndexData(IndexData::Primitive_LINES,skeletonIndexBuffer);
	mSkeletonVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION,mBones.size());
	mSkeletonVertexData=new VertexData(mSkeletonVertexBuffer);

	mHitBoxVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION,mBones.size()*8);
	mHitBoxVertexData=new VertexData(mHitBoxVertexBuffer);
	mHitBoxIndexData=new IndexData(IndexData::Primitive_LINES,NULL,0,mHitBoxVertexBuffer->getSize());

	updateSkeletonBuffers();
}

void SkeletonComponent::updateSkeletonBuffers(){
	Vector3 v;
	VertexBufferAccessor vba;
	vba.lock(mSkeletonVertexBuffer,Buffer::Access_BIT_WRITE);
	int i,j;
	for(i=0;i<mBones.size();++i){
		if(mBones[i]->useMatrixTransforms){
			Math::setTranslateFromMatrix4x4(v,mBones[i]->worldMatrix);
			vba.set3(i,0,v);
		}
		else{
			vba.set3(i,0,mBones[i]->worldTranslate);
		}
	}
	vba.unlock();

	vba.lock(mHitBoxVertexBuffer);
	Vector3 verts[8];
	for(i=0;i<mBones.size();++i){
		mBones[i]->worldBound.getVertexes(verts);
		for(j=0;j<8;++j){
			vba.set3(i*8+j,0,verts[j]);
		}
	}
	vba.unlock();
}

void SkeletonComponent::destroySkeletonBuffers(){
	if(mSkeletonVertexData!=NULL){
		mSkeletonVertexData->destroy();
		mSkeletonIndexData->destroy();
		mSkeletonVertexBuffer=NULL;
		mSkeletonVertexData=NULL;
		mSkeletonIndexData=NULL;
	}

	if(mHitBoxVertexData!=NULL){
		mHitBoxVertexData->destroy();
		mHitBoxIndexData->destroy();
		mHitBoxVertexBuffer=NULL;
		mHitBoxVertexData=NULL;
		mHitBoxIndexData=NULL;
	}
}

void SkeletonComponent::render(RenderManager *manager) const{
	manager->getDevice()->renderPrimitive(mSkeletonVertexData,mSkeletonIndexData);

	manager->getDevice()->renderPrimitive(mHitBoxVertexData,mHitBoxIndexData);
}

bool SkeletonComponent::getAttachmentTransform(Transform *result,int index){
	if(index>=0 && index<mBones.size()){
		Bone *bone=mBones[index];
		result->setTranslate(bone->worldTranslate);
		result->setRotate(bone->worldRotate);
		return true;
	}
	else{
		return false;
	}
}

Animation *SkeletonComponent::getAnimation(const String &name){
	int i;
	for(i=0;i<mAnimations.size();++i){
		if(mAnimations[i]->getName()==name){
			return mAnimations[i];
		}
	}
	return NULL;
}

}
}
