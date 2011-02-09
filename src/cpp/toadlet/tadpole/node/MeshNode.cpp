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

MeshNode::MeshNode():super()
	//mMesh,
	//mSubMeshes,
	//mSkeleton,
	//mDynamicVertexData,

	//mController
{}

Node *MeshNode::create(Scene *scene){
	super::create(scene);
	setCameraAligned(false);

	mMesh=NULL;
	mSubMeshes.clear();
	mSkeleton=NULL;
	mDynamicVertexData=NULL;

	mController=NULL;

	return this;
}

void MeshNode::destroy(){
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

	super::destroy();
}

Node *MeshNode::set(Node *node){
	super::set(node);

	MeshNode *meshNode=(MeshNode*)node;
	setMesh(meshNode->getMesh());

	return this;
}

void MeshNode::setMesh(const String &name){
	setMesh(mEngine->getMeshManager()->findMesh(name));
}

void MeshNode::setMesh(Mesh::ptr mesh){
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

	if(mesh==NULL){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"invalid Mesh");
		return;
	}

	mMesh=mesh;
	mMesh->retain();

	setTransform(mMesh->transform,0);
	setBound(mMesh->bound);

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
		subMesh->vertexData=mMesh->subMeshes[i]->vertexData;
		if(subMesh->vertexData==NULL){
			if(mDynamicVertexData!=NULL){
				subMesh->vertexData=mDynamicVertexData;
			}
			else{
				subMesh->vertexData=mMesh->staticVertexData;
			}
		}

		if(subMesh->meshSubMesh->transform!=NULL){
			subMesh->worldTransform=Transform::ptr(new Transform());
		}
		if(subMesh->meshSubMesh->bound!=NULL){
			subMesh->worldBound=Bound::ptr(new Bound());
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
		if(lastUpdateFrame==-1 || lastUpdateFrame==mScene->getFrame()){
			updateVertexBuffer();
		}
	}
}

void MeshNode::updateWorldTransform(){
	super::updateWorldTransform();

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh *subMesh=mSubMeshes[i];
		if(subMesh->worldTransform!=NULL){
			subMesh->worldTransform->transform(mWorldTransform,subMesh->worldTransform);

			if(subMesh->worldBound!=NULL){
				subMesh->worldBound->transform(subMesh->meshSubMesh->bound,subMesh->worldTransform);
			}
		}
		else if(subMesh->worldBound!=NULL){
			subMesh->worldBound->transform(subMesh->meshSubMesh->bound,mWorldTransform);
		}
	}
}

void MeshNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	int i;
	for(i=0;i<mSubMeshes.size();++i){
		SubMesh *subMesh=mSubMeshes[i];
		if(subMesh->worldBound!=NULL){
			if(camera->culled(subMesh->worldBound)==false){
				queue->queueRenderable(subMesh);
			}
		}
		else{
			queue->queueRenderable(subMesh);
		}
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
		VertexBuffer::ptr srcVertexBuffer=mMesh->staticVertexData->getVertexBuffer(0);
		VertexBuffer::ptr dstVertexBuffer=mDynamicVertexData->getVertexBuffer(0);

		VertexFormat *format=srcVertexBuffer->getVertexFormat();
		int positionIndex=format->findSemantic(VertexFormat::Semantic_POSITION);
		int normalIndex=format->findSemantic(VertexFormat::Semantic_NORMAL);

		{
			svba.lock(srcVertexBuffer,Buffer::Access_BIT_READ);
			dvba.lock(dstVertexBuffer,Buffer::Access_BIT_WRITE);

			// It appears that hardware buffers in opengl don't store their previous data if you want to write to them.
			// Basically, if you want to write any of it, you have to write it all.
			// So we re-copy over all the data to start
			// NOTE: Probably due to UT_WRITE_ONLY on creation, makes it mean if we lock it, we will rewrite all that data
			memcpy(dvba.getData(),svba.getData(),srcVertexBuffer->getDataSize());

			Vector3 &positionVector=cache_updateVertexBuffer_positionVector.reset();
			Vector3 &normalVector=cache_updateVertexBuffer_normalVector.reset();

			int i;
			if(normalIndex>=0){
				for(i=0;i<srcVertexBuffer->getSize();++i){
					svba.get3(i,positionIndex,positionVector);
					svba.get3(i,normalIndex,normalVector);

					const Mesh::VertexBoneAssignmentList &vba=mMesh->vertexBoneAssignments[i];
					if(vba.size()>0){
						MeshNodeSkeleton::Bone *bone=mSkeleton->getBone(mMesh->vertexBoneAssignments[i][0].bone); /// @todo: Implement all bones

						Math::mul(positionVector,bone->boneSpaceRotate);
						Math::add(positionVector,bone->boneSpaceTranslate);

						Math::mul(normalVector,bone->boneSpaceRotate);
					}
				
					dvba.set3(i,positionIndex,positionVector);
					dvba.set3(i,normalIndex,normalVector);
				}
			}
			else{
				for(i=0;i<srcVertexBuffer->getSize();++i){
					svba.get3(i,positionIndex,positionVector);

					const Mesh::VertexBoneAssignmentList &vba=mMesh->vertexBoneAssignments[i];
					if(vba.size()>0){
						MeshNodeSkeleton::Bone *bone=mSkeleton->getBone(mMesh->vertexBoneAssignments[i][0].bone); /// @todo: Implement all bones

						Math::mul(positionVector,bone->boneSpaceRotate);
						Math::add(positionVector,bone->boneSpaceTranslate);
					}

					dvba.set3(i,positionIndex,positionVector);
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
