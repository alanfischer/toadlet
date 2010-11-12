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
	mSequenceIndex=0;
	mSequenceTime=0;
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

	mTransformedVerts.resize(MAXSTUDIOVERTS);
	mTransformedNorms.resize(MAXSTUDIOVERTS);
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

	updateSkeleton();

	if(true){
		mSkeletonMaterial=mEngine->getMaterialManager()->createMaterial();
		mSkeletonMaterial->setDepthTest(Renderer::DepthTest_NONE);
		mSkeletonMaterial->setDepthWrite(false);
		mSkeletonMaterial->setPointParameters(true,8,false,0,0,0,0,0);
		createSkeletonBuffers();
	}
}

void StudioModelNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	mSequenceTime+=Math::fromMilli(dt)*20;
	if(mSequenceTime>=mModel->seqdesc(mSequenceIndex)->numframes-1){
		mSequenceTime=fmod(mSequenceTime,mModel->seqdesc(mSequenceIndex)->numframes-1);
	}
	activate();

	updateSkeleton();
}

void StudioModelNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	super::queueRenderables(camera,queue);

	Math::sub(mChromeForward,getWorldTranslate(),camera->getWorldTranslate());
	Math::normalize(mChromeForward);
	mChromeRight.set(camera->getRight());

	int i,j;
	for(i=0;i<=mModel->header->numbodyparts;++i){
		studiobodyparts *sbodyparts=mModel->bodyparts(i);
		for(j=0;j<sbodyparts->nummodels;++j){
			updateVertexes(mModel,i,j);
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

void StudioModelNode::updateVertexes(StudioModel *model,int bodypartsIndex,int modelIndex){
	int i,l;

	studiobodyparts *sbodyparts=model->bodyparts(bodypartsIndex);
	studiomodel *smodel=model->model(sbodyparts,modelIndex);
	tbyte *svertbone=((tbyte*)model->data+smodel->vertinfoindex);
	tbyte *snormbone=((tbyte*)model->data+smodel->norminfoindex);
	Vector3 *sverts=(Vector3*)(model->data+smodel->vertindex);
	Vector3 *snorms=(Vector3*)(model->data+smodel->normindex);

	Vector3 normal;
	Vector2 chrome;
	Vector3 *verts=mTransformedVerts;
	Vector3 *norms=mTransformedNorms;
	Vector2 *chromes=mTransformedChromes;
	Vector3 *boneTranslates=mBoneTranslates;
	Quaternion *boneRotates=mBoneRotates;

	for(i=0;i<smodel->numverts;++i){
		Math::mul(verts[i],boneRotates[svertbone[i]],sverts[i]);
		Math::add(verts[i],boneTranslates[svertbone[i]]);
	}

	for(i=0;i<smodel->numnorms;++i){
		Math::mul(norms[i],boneRotates[snormbone[i]],snorms[i]);
		if((model->bone(snormbone[i])->flags&STUDIO_HAS_CHROME)>0){
			findChrome(chromes[i],norms[i],mChromeForward,mChromeRight);
		}
	}

	vba.lock(model->vertexBuffer);

	for(i=0;i<smodel->nummesh;++i){
		studiomesh *smesh=model->mesh(smodel,i);
		StudioModel::meshdata *mdata=model->findmeshdata(bodypartsIndex,modelIndex,i);
		studiotexture *stexture=model->texture(model->skin(smesh->skinref));

		int indexDataCount=0;
		short *tricmds=(short*)(model->data+smesh->triindex);
		while(l=*(tricmds++)){
			int vertexCount=mdata->indexDatas[indexDataCount++]->start;

			if(l<0){
				l=-l;
			}
			for(;l>0;l--,tricmds+=4){
				vba.set3(vertexCount,0,verts[tricmds[0]]);
				vba.set3(vertexCount,1,norms[tricmds[1]]);
				if((stexture->flags&STUDIO_NF_CHROME)>0){
					vba.set2(vertexCount,2,chromes[tricmds[1]]);
				}
				vertexCount++;
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

void StudioModelNode::updateSkeleton(){
	studioseqdesc *sseqdesc=mModel->seqdesc(mSequenceIndex);

	mLocalBound.set(sseqdesc->bbmin,sseqdesc->bbmax);

	studioanim *sanim=mModel->anim(sseqdesc);
	findBoneTransforms(mBoneTranslates,mBoneRotates,mModel,sseqdesc,sanim,mSequenceTime);

	int i;
	for(i=0;i<mModel->header->numbones;++i){
		studiobone *sbone=mModel->bone(i);
		if(sbone->parent>=0){
			Math::preMul(mBoneRotates[i],mBoneRotates[sbone->parent]);
			Math::mul(mBoneTranslates[i],mBoneRotates[sbone->parent]);
			Math::add(mBoneTranslates[i],mBoneTranslates[sbone->parent]);
		}
	}
}

void StudioModelNode::findBoneTransforms(Vector3 *translates,Quaternion *rotates,StudioModel *model,studioseqdesc *sseqdesc,studioanim *sanim,float t){
	int frame=Math::intFloor(t);
	float s=(t-Math::fromInt(frame));

	// TODO: findBoneControllers??

	studiobone *sbone=model->bone(0);
	int i;
	for(i=0;i<model->header->numbones;++i,sbone++,sanim++){
		findBoneTranslate(translates[i],frame,s,sbone,sanim);
		findBoneRotate(rotates[i],frame,s,sbone,sanim);
	}
/*
	if((sseqdesc->motiontype&STUDIO_X)>0){
	if (pseqdesc->motiontype & STUDIO_X)
		pos[pseqdesc->motionbone][0] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Y)
		pos[pseqdesc->motionbone][1] = 0.0;
	if (pseqdesc->motiontype & STUDIO_Z)
		pos[pseqdesc->motionbone][2] = 0.0;
*/
}

void StudioModelNode::findBoneTranslate(Vector3 &r,int frame,float s,studiobone *sbone,studioanim *sanim){
	studioanimvalue *sanimvalue=NULL;
	int j,k;

	for(j=0;j<3;j++){
		r[j]=sbone->value[j];
		if(sanim->offset[j]!=0){
			sanimvalue=mModel->animvalue(sanim,j);
			k=frame;
			while(sanimvalue->num.total<=k){
				k-=sanimvalue->num.total;
				sanimvalue+=sanimvalue->num.valid+1;
			}
			if(sanimvalue->num.valid>k){
				if(sanimvalue->num.valid>k+1){
					r[j]+=(sanimvalue[k+1].value*(1.0-s)+s*sanimvalue[k+2].value)*sbone->scale[j];
				}
				else{
					r[j]+=sanimvalue[k+1].value*sbone->scale[j];
				}
			}
			else{
				if(sanimvalue->num.total<=k+1){
					r[j]+=(sanimvalue[sanimvalue->num.valid].value*(1.0-s)+s*sanimvalue[sanimvalue->num.valid+2].value)*sbone->scale[j];
				}
				else{
					r[j]+=sanimvalue[sanimvalue->num.valid].value*sbone->scale[j];
				}
			}
		}
		if(sbone->bonecontroller[j]!=-1){
//			r[j]+=mAdj[sbone->bonecontroller[j]];
		}
	}
}

void StudioModelNode::findBoneRotate(Quaternion &r,int frame,float s,studiobone *sbone,studioanim *sanim){
	Quaternion q1,q2;
	EulerAngle angle1,angle2;
	studioanimvalue *sanimvalue=NULL;
	int j,k;

	for(j=0;j<3;++j){
		if(sanim->offset[j+3]==0){
			angle2[j]=angle1[j]=sbone->value[j+3];
		}
		else{
			sanimvalue=mModel->animvalue(sanim,j+3);
			k=frame;
			while(sanimvalue->num.total<=k){
				k-=sanimvalue->num.total;
				sanimvalue+=sanimvalue->num.valid+1;
			}
			if(sanimvalue->num.valid>k){
				angle1[j]=sanimvalue[k+1].value;
				if(sanimvalue->num.valid>k+1){
					angle2[j]=sanimvalue[k+2].value;
				}
				else{
					if(sanimvalue->num.total>k+1){
						angle2[j]=angle1[j];
					}
					else{
						angle2[j]=sanimvalue[sanimvalue->num.valid+2].value;
					}
				}
			}
			else{
				angle1[j]=sanimvalue[sanimvalue->num.valid].value;
				if(sanimvalue->num.total>k+1){
					angle2[j]=angle1[j];
				}
				else{
					angle2[j]=sanimvalue[sanimvalue->num.valid+2].value;
				}
			}
			angle1[j]=sbone->value[j+3]+angle1[j]*sbone->scale[j+3];
			angle2[j]=sbone->value[j+3]+angle2[j]*sbone->scale[j+3];
		}
		if(sbone->bonecontroller[j+3]!=-1){
//			angle1[j]+=mAdj[sbone->bonecontroller[j+3]];
//			angle2[j]+=mAdj[sbone->bonecontroller[j+3]];
		}
	}

	if(angle1.equals(angle2)==false){
		setQuaternionFromEulerAngleStudio(q1,angle1);
		setQuaternionFromEulerAngleStudio(q2,angle2);
		Math::slerp(r,q1,q2,s);
	}
	else{
		setQuaternionFromEulerAngleStudio(r,angle1);
	}
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

void StudioModelNode::setQuaternionFromEulerAngleStudio(Quaternion &r,const EulerAngle &euler){
		real sx=euler.x/2;
		real cx=cos(sx);
		sx=sin(sx);
		real sy=euler.y/2;
		real cy=cos(sy);
		sy=sin(sy);
		real sz=euler.z/2;
		real cz=cos(sz);
		sz=sin(sz);
		real cxcy=cx*cy;
		real sxsy=sx*sy;
		real cxsy=cx*sy;
		real sxcy=sx*cy;

		r.x=(sxcy*cz) - (cxsy*sz);
		r.y=(cxsy*cz) + (sxcy*sz);
  		r.z=(cxcy*sz) - (sxsy*cz);
		r.w=(cxcy*cz) + (sxsy*sz);
}

}
}
}
