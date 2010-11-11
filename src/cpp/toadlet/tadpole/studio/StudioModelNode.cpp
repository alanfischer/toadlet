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

#include <toadlet/peeper/VertexFormat.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/studio/StudioModelNode.h>
#include <toadlet/tadpole/studio/StudioHandler.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace studio{

TOADLET_NODE_IMPLEMENT(StudioModelNode,Categories::TOADLET_TADPOLE_STUDIO+".StudioModelNode");

StudioModelNode::SubModel::SubModel(StudioModelNode *modelNode,int bodypartIndex,int modelIndex,int meshIndex){
	this->modelNode=modelNode;
	this->bodypartIndex=bodypartIndex;
	this->modelIndex=modelIndex;
	this->meshIndex=meshIndex;

	model=modelNode->getModel();
	sbodyparts=model->bodyparts(bodypartIndex);
	smodel=model->model(sbodyparts,modelIndex);
	smesh=model->mesh(smodel,meshIndex);
	mdata=model->findmeshdata(bodypartIndex,modelIndex,meshIndex);
	material=model->materials[smesh->skinref];
}

void StudioModelNode::SubModel::render(Renderer *renderer) const{
	int i;
	for(i=0;i<mdata->indexDatas.size();++i){
		renderer->renderPrimitive(model->vertexData,mdata->indexDatas[i]);
	}
}

StudioModelNode::StudioModelNode():super(){
}

StudioModelNode::~StudioModelNode(){
}

void StudioModelNode::destroy(){
	if(mModel!=NULL){
		mModel->release();
		mModel=NULL;
	}

	if(mSkeletonMaterial!=NULL){
		mSkeletonMaterial->release();
		mSkeletonMaterial=NULL;
	}

	if(mSkeletonVertexData!=NULL){
		mSkeletonVertexData->destroy();
		mSkeletonIndexData->destroy();
		mSkeletonVertexBuffer=NULL;
		mSkeletonVertexData=NULL;
		mSkeletonIndexData=NULL;
	}

	super::destroy();
}

void StudioModelNode::setModel(StudioModel::ptr model){
	mSubModels.clear();

	mModel=model;

	mBoneTranslates.resize(model->header->numbones);
	mBoneRotates.resize(model->header->numbones);

	mTransformedPoints.resize(MAXSTUDIOVERTS);
	mTransformedNormals.resize(MAXSTUDIOVERTS);
	mTransformedChromes.resize(MAXSTUDIOVERTS);

	int i,j,k;
	for(i=0;i<=model->header->numbodyparts;++i){
		studiobodyparts *sbodyparts=model->bodyparts(i);
		for(j=0;j<sbodyparts->nummodels;++j){
			studiomodel *smodel=model->model(sbodyparts,j);
			for(k=0;k<smodel->nummesh;++k){
				SubModel::ptr subModel(new SubModel(this,i,j,k));
				mSubModels.add(subModel);
			}
		}
	}

	studioseqdesc *sseqdesc=model->seqdesc(0);
	mLocalBound.set(sseqdesc->bbmin,sseqdesc->bbmax);

	mSkeletonMaterial=mEngine->getMaterialManager()->createMaterial();
	mSkeletonMaterial->setDepthTest(Renderer::DepthTest_NONE);
	mSkeletonMaterial->setDepthWrite(false);
	mSkeletonMaterial->setPointParameters(true,8,false,0,0,0,0,0);
	createSkeletonBuffers();
}

void StudioModelNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	int i,j;
	for(i=0;i<mModel->header->numtextures;++i){
		if((mModel->texture(i)->flags&STUDIO_NF_CHROME)>0){
			break;
		}
	}
	if(i<mModel->header->numtextures){
		for(i=0;i<=mModel->header->numbodyparts;++i){
			studiobodyparts *sbodyparts=mModel->bodyparts(i);
			for(j=0;j<sbodyparts->nummodels;++j){
				updateChrome(mModel,i,j,camera);
			}
		}
	}

	for(i=0;i<mSubModels.size();++i){
		queue->queueRenderable(mSubModels[i]);
	}

	if(mSkeletonMaterial!=NULL){
		updateSkeletonBuffers();
		queue->queueRenderable(this);
	}
}

void StudioModelNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mSkeletonVertexData,mSkeletonIndexData);
	mSkeletonIndexData->primitive=IndexData::Primitive_POINTS;
	renderer->renderPrimitive(mSkeletonVertexData,mSkeletonIndexData);
	mSkeletonIndexData->primitive=IndexData::Primitive_LINES;
}

void StudioModelNode::updateChrome(StudioModel *model,int bodypartsIndex,int modelIndex,CameraNode *camera){
	int i,l;

	Vector3 forward,right;
	Math::sub(forward,getWorldTranslate(),camera->getWorldTranslate());
	Math::normalize(forward);
	right.set(camera->getRight());

	Vector3 normal;
	Vector2 chrome;
	Vector3 *normals=&mTransformedNormals[0];
	Vector2 *chromes=&mTransformedChromes[0];

	studiobodyparts *sbodyparts=model->bodyparts(bodypartsIndex);
	studiomodel *smodel=model->model(sbodyparts,modelIndex);

	// HACK: Just copy over the norms till we properly calculate them
	memcpy(normals,model->data+smodel->normindex,smodel->numnorms*sizeof(Vector3));

	vba.lock(model->vertexBuffer);

	for(i=0;i<smodel->numnorms;++i){
		// TODO: Just chrome the norms with bones that have the chrome flag
		findChrome(chromes[i],normals[i],forward,right);
	}

	for(i=0;i<smodel->nummesh;++i){
		studiomesh *smesh=model->mesh(smodel,i);
		StudioModel::meshdata *mdata=model->findmeshdata(bodypartsIndex,modelIndex,i);
		studiotexture *stexture=model->texture(model->skin(smesh->skinref));
		Vector3 *norms=(Vector3*)(model->data+smodel->normindex);

		if((stexture->flags&STUDIO_NF_CHROME)>0){
			int indexDataCount=0;
			short *tricmds=(short*)(model->data+smesh->triindex);
			while(l=*(tricmds++)){
				int vertexCount=mdata->indexDatas[indexDataCount++]->start;

				if(l<0){
					l=-l;
				}
				for(;l>0;l--,tricmds+=4){
					vba.set2(vertexCount,2,chromes[tricmds[1]]);
					vertexCount++;
				}
			}
		}
	}

	vba.unlock();
}

void StudioModelNode::findChrome(Vector2 &chrome,const Vector3 &normal,const Vector3 &forward,const Vector3 &right){
	Vector3 chromeUp,chromeRight,temp;
	float n=0;

	Math::cross(chromeUp,right,forward);
	Math::normalize(chromeUp);
	Math::cross(chromeRight,temp,chromeUp);
	Math::normalize(chromeRight);

	// TODO: Rotate chrome vectors by bone transform

	n=Math::dot(normal,chromeRight);
	chrome.x=(n+1.0f)*32.0f;
	n=Math::dot(normal,chromeUp);
	chrome.y=(n+1.0f)*32.0f;
}

void StudioModelNode::createSkeletonBuffers(){
	int i;

	int indexCount=0;
	for(i=0;i<mModel->header->numbones;++i){
		studiobone *bone=mModel->bone(i);
		if(bone->parent>=0){
			indexCount+=2;
		}
	}

	VertexBuffer::ptr skeletonVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION,mModel->header->numbones);
	IndexBuffer::ptr skeletonIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,indexCount);

	iba.lock(skeletonIndexBuffer);

	indexCount=0;
	for(i=0;i<mModel->header->numbones;++i){
		studiobone *bone=mModel->bone(i);
		if(bone->parent>=0){
			iba.set(indexCount++,bone->parent);
			iba.set(indexCount++,i);
		}
	}
	
	iba.unlock();

	mSkeletonVertexBuffer=skeletonVertexBuffer;
	mSkeletonVertexData=VertexData::ptr(new VertexData(mSkeletonVertexBuffer));
	mSkeletonIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINES,skeletonIndexBuffer));
}

void StudioModelNode::updateSkeletonBuffers(){
	vba.lock(mSkeletonVertexBuffer);

	int i;
	for(i=0;i<mModel->header->numbones;++i){
		studiobone *bone=mModel->bone(i);
		vba.set3(i,0,mBoneTranslates[i]);
	}

	vba.unlock();
}

}
}
}
