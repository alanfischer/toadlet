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

#include <toadlet/tadpole/node/MeshNodeSkeleton.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/node/Scene.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{
namespace node{

MeshNodeSkeleton::MeshNodeSkeleton(MeshNode* node,Skeleton::ptr skeleton):
	//mSkeleton,
	//mBones,
	mLastRenderUpdateFrame(-1),

	//mSequence,
	mSequenceTime(0)
{
	mNode=node;
	mSkeleton=skeleton;

	mBones.resize(skeleton->bones.size());
	int i;
	for(i=0;i<skeleton->bones.size();++i){
		mBones[i]=Bone::ptr(new Bone(i));
	}

	mTrackHints.resize(mBones.size());

	updateBones();
}

void MeshNodeSkeleton::updateBones(){
	int i;
	for(i=0;i<mBones.size();++i){
		updateBone(mBones[i]);
	}

	if(mVertexData!=NULL){
		updateVertexData();
	}

	if(mNode!=NULL && mNode->getEngine()!=NULL && mNode->getEngine()->getScene()!=NULL){
		mLastRenderUpdateFrame=mNode->getEngine()->getScene()->getRenderFrame();
	}
	else{
		mLastRenderUpdateFrame=-1;
	}
}

void MeshNodeSkeleton::updateBones(int sequenceIndex,scalar sequenceTime){
	if(sequenceIndex>=0 && sequenceIndex<mSkeleton->sequences.size()){
		mSequence=mSkeleton->sequences[sequenceIndex];
		mSequenceTime=sequenceTime;
	}

	updateBones();
}

MeshNodeSkeleton::Bone *MeshNodeSkeleton::getBone(const String &name) const{
	int i;
	for(i=0;i<mSkeleton->bones.size();++i){
		if(mSkeleton->bones[i]->name.equals(name)){
			return mBones[i];
		}
	}

	return NULL;
}

String MeshNodeSkeleton::getBoneName(int index) const{
	if(index>=0 && index<mSkeleton->bones.size()){
		return mSkeleton->bones[index]->name;
	}
	else{
		return "";
	}
}

String MeshNodeSkeleton::getBoneName(Bone *bone) const{
	return mSkeleton->bones[bone->index]->name;
}

int MeshNodeSkeleton::updateBoneTransformation(Bone *bone){
	if(mSequence!=NULL){
		Track *track=mSequence->tracks[bone->index];
		scalar time=mSequenceTime;
		if(track!=NULL){
			const KeyFrame *f1=NULL,*f2=NULL;
			scalar t=track->getKeyFramesAtTime(time,f1,f2,mTrackHints[bone->index]);
			if(f1==NULL || f2==NULL){
				return bone->dontUpdateFlags;
			}
			if((bone->dontUpdateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
				Math::lerp(bone->localTranslate,f1->translate,f2->translate,t);
			}
			if((bone->dontUpdateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
				Math::lerp(bone->localRotate,f1->rotate,f2->rotate,t);
				Math::normalizeCarefully(bone->localRotate,0);
			}
			return BoneSpaceUpdate_FLAG_TRANSLATE|BoneSpaceUpdate_FLAG_ROTATE;
		}
	}

	return bone->dontUpdateFlags;
}

void MeshNodeSkeleton::updateBone(Bone *bone){
	int updateFlags=BoneSpaceUpdate_NONE;

	Skeleton::Bone *sbone=mSkeleton->bones[bone->index];

	if(bone->controller==NULL){
		updateFlags=updateBoneTransformation(bone);
	}
	else{
		updateFlags=bone->controller->updateBoneTransform(bone);
	}

	bone->useMatrixTransforms=((updateFlags&BoneSpaceUpdate_FLAG_MATRIX)>0);

	Bone *parent=NULL;
	int parentIndex=sbone->parentIndex;
	if(parentIndex>=0){
		parent=mBones[parentIndex];

		// If a parent only uses matrix transforms, then this bone must do it also, since the quaternions are not available
		bone->useMatrixTransforms|=parent->useMatrixTransforms;
	}

	Vector3 &vector=cache_updateBone_vector.reset();
	Quaternion &quaternion=cache_updateBone_quaternion.reset();

	if((updateFlags&BoneSpaceUpdate_FLAG_WORLD)==0){
		if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
			bone->localRotate.set(sbone->rotate);
		}

		if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
			bone->localTranslate.set(sbone->translate);
		}

		if(bone->useMatrixTransforms){
			if(parent==NULL){
				Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->localRotate);

				bone->worldTranslate.set(bone->localTranslate);
			}
			else{
				Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->localRotate);
				Math::preMul(bone->worldRotateMatrix,parent->worldRotateMatrix);

				Math::mul(vector,parent->worldRotateMatrix,bone->localTranslate);
				Math::add(bone->worldTranslate,parent->worldTranslate,vector);
			}
		}
		else{
			if(parent==NULL){
				bone->worldRotate.set(bone->localRotate);
				Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->worldRotate);

				bone->worldTranslate.set(bone->localTranslate);
			}
			else{
				Math::mul(bone->worldRotate,parent->worldRotate,bone->localRotate);
				Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->worldRotate);

				Math::mul(vector,parent->worldRotateMatrix,bone->localTranslate);
				Math::add(bone->worldTranslate,parent->worldTranslate,vector);
			}
		}
	}
	else{
		if(bone->useMatrixTransforms){
			if(parent==NULL){
				if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
					Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,sbone->rotate);
				}
				else{
					if((updateFlags&BoneSpaceUpdate_FLAG_MATRIX)==0){
						Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->worldRotate);
					}
				}

				if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
					bone->worldTranslate.set(sbone->translate);
				}
			}
			else{
				if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
					Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,sbone->rotate);
					Math::preMul(bone->worldRotateMatrix,parent->worldRotateMatrix);
				}
				else{
					if((updateFlags&BoneSpaceUpdate_FLAG_MATRIX)==0){
						Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->worldRotate);
					}
				}

				if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
					Math::mul(vector,parent->worldRotateMatrix,sbone->translate);
					Math::add(bone->worldTranslate,parent->worldTranslate,vector);
				}
			}
		}
		else{
			if(parent==NULL){
				if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
					bone->worldRotate.set(sbone->rotate);
				}
				Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->worldRotate);

				if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
					bone->worldTranslate.set(sbone->translate);
				}
			}
			else{
				if((updateFlags&BoneSpaceUpdate_FLAG_ROTATE)==0){
					Math::mul(bone->worldRotate,parent->worldRotate,sbone->rotate);
				}
				Math::setMatrix3x3FromQuaternion(bone->worldRotateMatrix,bone->worldRotate);

				if((updateFlags&BoneSpaceUpdate_FLAG_TRANSLATE)==0){
					Math::mul(vector,parent->worldRotateMatrix,sbone->translate);
					Math::add(bone->worldTranslate,parent->worldTranslate,vector);
				}
			}
		}
	}

	Math::mul(vector,bone->worldRotateMatrix,sbone->worldToBoneTranslate);
	Math::add(bone->boneSpaceTranslate,bone->worldTranslate,vector);

	if(bone->useMatrixTransforms){
		Math::setMatrix3x3FromQuaternion(bone->boneSpaceRotate,sbone->worldToBoneRotate);
		Math::preMul(bone->boneSpaceRotate,bone->worldRotateMatrix);
	}
	else{
		Math::mul(quaternion,bone->worldRotate,sbone->worldToBoneRotate);
		Math::setMatrix3x3FromQuaternion(bone->boneSpaceRotate,quaternion);
	}
}

void MeshNodeSkeleton::setRenderable(bool renderable){
	if(renderable){
		Engine *engine=mNode->getEngine();
		int i;
		mMaterial=engine->getMaterialManager()->createMaterial();
		mMaterial->retain();
		mMaterial->setLighting(true);
		LightEffect lightEffect(Colors::GREEN);
		lightEffect.emissive.set(Colors::GREEN);
		mMaterial->setLightEffect(lightEffect);
		mMaterial->setDepthTest(Renderer::DepthTest_NONE);
		mMaterial->setLayer(1);

		IndexBuffer::ptr indexBuffer=engine->getBufferManager()->createIndexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,(mBones.size()-1)*2);
		IndexBufferAccessor iba;
		{
			iba.lock(indexBuffer,Buffer::AccessType_WRITE_ONLY);
			for(i=1;i<mBones.size();++i){
				iba.set((i-1)*2+0,mSkeleton->bones[i]->parentIndex<0?0:mSkeleton->bones[i]->parentIndex);
				iba.set((i-1)*2+1,i);
			}
			iba.unlock();
		}
		mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINES,indexBuffer));

		VertexBuffer::ptr vertexBuffer=engine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_DYNAMIC,Buffer::AccessType_WRITE_ONLY,engine->getVertexFormats().POSITION,mBones.size());
		mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

		updateVertexData();
	}
	else{
		mMaterial=NULL;
		mIndexData=NULL;
		mVertexData=NULL;
	}
}

void MeshNodeSkeleton::updateVertexData(){
	VertexBufferAccessor vba;
	vba.lock(mVertexData->getVertexBuffer(0),Buffer::AccessType_WRITE_ONLY);
	int i;
	for(i=0;i<mBones.size();++i){
		vba.set3(i,0,mBones[i]->worldTranslate);
	}
	vba.unlock();
}

const Matrix4x4 &MeshNodeSkeleton::getRenderTransform() const{
	return mNode!=NULL?mNode->getRenderWorldTransform():Math::IDENTITY_MATRIX4X4;
}

void MeshNodeSkeleton::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

}
}
}
