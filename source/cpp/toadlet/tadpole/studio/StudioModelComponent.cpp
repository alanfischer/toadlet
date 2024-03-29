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
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/studio/StudioModelComponent.h>

namespace toadlet{
namespace tadpole{
namespace studio{

StudioModelComponent::SubModel::SubModel(StudioModelComponent *parent,int bodypartIndex,int modelIndex,int meshIndex,int skinIndex){
	this->mParent=parent;
	this->mBodypartIndex=bodypartIndex;
	this->mModelIndex=modelIndex;
	this->mMeshIndex=meshIndex;

	mModel=parent->getModel();
	mSBodyparts=mModel->bodyparts(mBodypartIndex);
	mSModel=mModel->model(mSBodyparts,mModelIndex);
	mSMesh=mModel->mesh(mSModel,mMeshIndex);
	mData=mModel->findmeshdata(mBodypartIndex,mModelIndex,mMeshIndex);
	mMaterial=mModel->materials[mModel->skin(mModel->header->numskinref*skinIndex+mSMesh->skinref)];
}

void StudioModelComponent::SubModel::render(RenderManager *manager) const{
	int i;
	for(i=0;i<mData->indexDatas.size();++i){
		manager->getDevice()->renderPrimitive(mParent->mVertexData,mData->indexDatas[i]);
	}
}


StudioModelComponent::SequenceAnimation::SequenceAnimation(StudioModelComponent *parent,studioseqdesc *sequence):
	mSSequence(sequence),
	mValue(0),
	mWeight(0),
	mScope(-1)
{
	mParent=parent;
	mName=sequence->label;
}


StudioModelComponent::StudioModelComponent(Engine *engine):
	mRendered(false),
	//mModel,
	//mSubModels,
	//mSharedRenderState,

	mBodypartIndex(0),
	mModelIndex(0),
	mSkinIndex(0),
	//mControllerValues[4],mAdjustedControllerValues[4],
	//mBlenderValues[4],mAdjustedBlenderValues[4],
	//mLink,
	//mLinkModel,
	mBlendSequenceIndex(0),
	mSkeletonDirty(false),
	mMeshDirty(false)

	//mChromeForward,mChromeRight,
	//mBoneTranslates,
	//mBoneRotates,
	//mBoneLinks,
	//mTransformedVerts,
	//mTransformedNorms,
	//mTransformedChromes,
	//mVertexBuffer,
	//mVertexData,

	//mSkeletonMaterial,
	//mSkeletonVertexBuffer,
	//mSkeletonVertexData,
	//mSkeletonIndexData,
	//mHitBoxVertexBuffer,
	//mHitBoxVertexData,
	//mHitBoxIndexData
{
	memset(mControllerValues,0,sizeof(mControllerValues));
	memset(mAdjustedControllerValues,0,sizeof(mAdjustedControllerValues));
	mRendered=true;
	mEngine=engine;
	mTransform=new Transform();
	mTransform->setTransformListener(this);
	mWorldTransform=new Transform();
	mBound=new Bound();
	mWorldBound=new Bound();
}

void StudioModelComponent::destroy(){
	int i;
	for(i=0;i<mSubModels.size();++i){
		mSubModels[i]->destroy();
	}
	mSubModels.clear();

	if(mSkeletonMaterial!=NULL){
		mSkeletonMaterial->destroy();
		mSkeletonMaterial=NULL;

		destroySkeletonBuffers();
	}

	mModel=NULL;

	BaseComponent::destroy();
}

void StudioModelComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
		mParent->visibleRemoved(this);
		mParent->animatableRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->spacialAttached(this);
		mParent->visibleAttached(this);
		mParent->animatableAttached(this);
	}
}

void StudioModelComponent::logicUpdate(int dt,int scope){
	if(mSkeletonDirty){
		updateSkeleton();
	}
}

void StudioModelComponent::frameUpdate(int dt,int scope){
	if(mSkeletonDirty || mParent->getScene()->getResetFrame()){
		updateSkeleton();
	}
}

void StudioModelComponent::setModel(const String &name){
	mEngine->getStudioModelManager()->find(name,this);
}

void StudioModelComponent::setModel(StudioModel *model){
	mModel=model;

	if(mModel==NULL){
		return;
	}

	mBoneTranslates.resize(model->header->numbones);
	mBoneRotates.resize(model->header->numbones);
	mBlendBoneTranslates.resize(model->header->numbones);
	mBlendBoneRotates.resize(model->header->numbones);
	mBoneLinks.resize(mModel->header->numbones,-1);
	mBoneScopes.resize(mModel->header->numbones,-1);

	mTransformedVerts.resize(MAXSTUDIOVERTS);
	mTransformedNorms.resize(MAXSTUDIOVERTS);
	mTransformedChromes.resize(MAXSTUDIOVERTS);

	mVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,model->vertexCount);
	mVertexData=new VertexData(mVertexBuffer);

	createSubModels();

	setLink(mLink);
	setRenderSkeleton(mSkeletonMaterial!=NULL);

	mAnimations.resize(mModel->header->numseq);
	int i;
	for(i=0;i<mModel->header->numseq;++i){
		mAnimations[i]=new SequenceAnimation(this,mModel->seqdesc(i));
	}

	updateSkeleton();
}

void StudioModelComponent::setRenderSkeleton(bool skeleton){
	if(skeleton){
		if(mSkeletonMaterial==NULL){
			RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
			renderState->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			renderState->setGeometryState(GeometryState(true,8,false,0,0,0,0,0));
			mSkeletonMaterial=mEngine->createDiffuseMaterial(NULL,renderState);
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

void StudioModelComponent::setController(int controller,scalar v){
	studiobonecontroller *sbonecontroller=NULL;
	int i;
	for(i=0;i<mModel->header->numbonecontrollers;++i){
		sbonecontroller=mModel->bonecontroller(i);
		if(sbonecontroller->index==controller) break;
	}
	if(sbonecontroller==NULL){
		return;
	}

	if((sbonecontroller->type&(STUDIO_XR|STUDIO_YR|STUDIO_ZR))>0){
		if(sbonecontroller->end<sbonecontroller->start){
			v=-v;
		}

		v=fmod(v,Math::TWO_PI);
	}

	mControllerValues[controller]=v;

	if((sbonecontroller->type&(STUDIO_XR|STUDIO_YR|STUDIO_ZR))>0){
		v=Math::clamp(Math::degToRad(sbonecontroller->start),Math::degToRad(sbonecontroller->end),v);
	}
	if((sbonecontroller->type&(STUDIO_X|STUDIO_Y|STUDIO_Z))>0){
		v=(Math::ONE-v)*sbonecontroller->start+v*sbonecontroller->end;
	}

	mAdjustedControllerValues[controller]=v;
}

void StudioModelComponent::setBlender(int blender,scalar v){
	studioseqdesc *sseqdesc=mModel->seqdesc(mBlendSequenceIndex);
	if(blender<0 || blender>=sseqdesc->numblends){
		return;
	}

	if((sseqdesc->blendtype[blender]&(STUDIO_XR|STUDIO_YR|STUDIO_ZR))>0){
		if(sseqdesc->blendend[blender]<sseqdesc->blendstart[blender]){
			v=-v;
		}

		v=fmod(v,Math::TWO_PI);
	}

	mBlenderValues[blender]=v;

	if((sseqdesc->blendtype[blender]&(STUDIO_XR|STUDIO_YR|STUDIO_ZR))>0){
		v=Math::clamp(Math::degToRad(sseqdesc->blendstart[blender]),Math::degToRad(sseqdesc->blendend[blender]),v);
		v=(v-Math::degToRad(sseqdesc->blendstart[blender]))/Math::degToRad(sseqdesc->blendend[blender]-sseqdesc->blendstart[blender]);
	}

	mAdjustedBlenderValues[blender]=v;
}

void StudioModelComponent::setBodypart(int part){
	mBodypartIndex=part;

	createSubModels();
}

void StudioModelComponent::setBodypartModel(int model){
	mModelIndex=model;

	createSubModels();
}

void StudioModelComponent::setSkin(int skin){
	if(skin<0 || skin>=mModel->header->numskinfamilies){
		skin=0;
	}

	mSkinIndex=skin;

	createSubModels();
}

int StudioModelComponent::getBoneIndex(const String &name){
	int i;
	for(i=0;i<mModel->header->numbones;++i){
		studiobone *bone=mModel->bone(i);
		if(name==bone->name){
			return i;
		}
	}
	return -1;
}

void StudioModelComponent::setBoneScope(int i,int scope,bool recurse){
	if(i==-1){
		for(i=0;i<mModel->header->numbones;++i){
			studiobone *bone=mModel->bone(i);
			if(bone->parent==-1){
				setBoneScope(i,scope,recurse);
			}
		}
		return;
	}

	mBoneScopes[i]=scope;

	if(recurse){
		int parent=i;
		for(i=0;i<mModel->header->numbones;++i){
			studiobone *bone=mModel->bone(i);
			if(bone->parent==parent){
				setBoneScope(i,scope,recurse);
			}
		}
	}
}

void StudioModelComponent::setLink(StudioModelComponent *link){
	mLink=link;

	if(mLink==NULL){
		return;
	}

	mLinkModel=mLink->mModel;

	if(mModel==NULL || mLinkModel==NULL){
		return;
	}

	mBoneLinks.clear();
	mBoneLinks.resize(mModel->header->numbones,-1);

	int i,j;
	for(i=0;i<mModel->header->numbones;++i){
		for(j=0;j<mLink->mModel->header->numbones;++j){
			if(strcmp(mModel->bone(i)->name,mLink->mModel->bone(j)->name)==0){
				mBoneLinks[i]=j;
				break;
			}
		}
	}
}

void StudioModelComponent::setTransform(Transform::ptr transform){
	if(transform==NULL){
		mTransform->reset();
	}
	else{
		mTransform->set(transform);
	}
}

void StudioModelComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	mWorldTransform->setTransform(mParent->getWorldTransform(),mTransform);
	mWorldBound->transform(mBound,mWorldTransform);

	if(transform==mTransform){
		mParent->boundChanged();
	}
}

void StudioModelComponent::resourceReady(Resource *resource){
	setModel((StudioModel*)resource);
}

void StudioModelComponent::traceDetailSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	Vector3 point,normal;
	Segment localSegment,boxSegment;
	Quaternion invrot;
	Transform transform;
	int i;

	transform.set(position,mParent->getWorldTransform()->getScale(),mParent->getWorldTransform()->getRotate());
	transform.inverseTransform(localSegment,segment);

	result.time=Math::ONE;
	for(i=0;i<mModel->header->numhitboxes;++i){
		studiobbox *sbbox=mModel->bbox(i);
		Math::invert(invrot,mBoneRotates[sbbox->bone]);
		Math::mul(boxSegment.direction,invrot,localSegment.direction);
		Math::sub(boxSegment.origin,localSegment.origin,mBoneTranslates[sbbox->bone]);
		Math::mul(boxSegment.origin,invrot);
		scalar time=Math::findIntersection(boxSegment,AABox(sbbox->bbmin,sbbox->bbmax),point,normal);
		if(time<result.time){
			result.time=time;
			Math::mul(result.point,mBoneRotates[sbbox->bone],point);
			Math::add(result.point,mBoneTranslates[sbbox->bone]);
			Math::mul(result.normal,mBoneRotates[sbbox->bone],normal);
			result.index=i;
		}
	}

	if(result.time<Math::ONE){
		transform.transform(result.point);
		transform.transformNormal(result.normal);
	}
}

void StudioModelComponent::setSharedRenderState(RenderState::ptr renderState){
	int i;
	if(renderState==NULL){
		renderState=mEngine->getMaterialManager()->createRenderState();
		for(i=0;i<mSubModels.size();++i){
			if(i==0){
				mEngine->getMaterialManager()->modifyRenderState(renderState,mSubModels[i]->mMaterial->getRenderState());
			}
		}
	}

	mSharedRenderState=renderState;
	for(i=0;i<mSubModels.size();++i){
		mSubModels[i]->mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mSubModels[i]->mMaterial,mSharedRenderState);
	}
}

void StudioModelComponent::gatherRenderables(Camera *camera,RenderableSet *set){
	if(mModel==NULL || mRendered==false){
		return;
	}

	Math::sub(mChromeForward,mParent->getWorldTranslate(),camera->getPosition());
	Math::normalize(mChromeForward);
	mChromeRight.set(camera->getRight());

	int i,j;
	if(mMeshDirty){
		for(i=0;i<=mModel->header->numbodyparts;++i){
			studiobodyparts *sbodyparts=mModel->bodyparts(i);
			for(j=0;j<sbodyparts->nummodels;++j){
				updateVertexes(mModel,i,j);
			}
		}
		mMeshDirty=false;
	}

	for(i=0;i<mSubModels.size();++i){
		set->queueRenderable(mSubModels[i]);
	}

	if(mSkeletonMaterial!=NULL){
		updateSkeletonBuffers();
		set->queueRenderable(this);
	}
}

void StudioModelComponent::render(RenderManager *manager) const{
	manager->getDevice()->renderPrimitive(mSkeletonVertexData,mSkeletonIndexData);

	mSkeletonIndexData->primitive=IndexData::Primitive_POINTS;
	manager->getDevice()->renderPrimitive(mSkeletonVertexData,mSkeletonIndexData);
	mSkeletonIndexData->primitive=IndexData::Primitive_LINES;

	manager->getDevice()->renderPrimitive(mHitBoxVertexData,mHitBoxIndexData);
}

int StudioModelComponent::getAttachmentIndex(const String &name) const{
	int i;
	for(i=0;i<mModel->header->numattachments;++i){
		if(name.equals(mModel->attachment(i)->name)){
			return i;
		}
	}

	return -1;
}

bool StudioModelComponent::getAttachmentTransform(Transform *result,int index) const{
	if(index<0 || index>mModel->header->numattachments){
		return false;
	}

	studioattachment *attachment=mModel->attachment(index);
	Vector3 translate=mBoneTranslates[attachment->bone];
	Quaternion rotate=mBoneRotates[attachment->bone];

	Math::add(translate,attachment->org);
	result->setTranslate(translate);

	Quaternion attachmentRotate;
	Math::setQuaternionFromAxes(attachmentRotate,attachment->vectors[0],attachment->vectors[1],attachment->vectors[2]);
	Math::postMul(rotate,attachmentRotate);
	result->setRotate(rotate);

	return true;
}

Animation *StudioModelComponent::getAnimation(const String &name) const{
	int i;
	for(i=0;i<mAnimations.size();++i){
		if(mAnimations[i]->getName()==name){
			return mAnimations[i];
		}
	}
	return NULL;
}

void StudioModelComponent::updateVertexes(StudioModel *model,int bodypartsIndex,int modelIndex){
	int i,l;

	studiobodyparts *sbodyparts=model->bodyparts(bodypartsIndex);
	studiomodel *smodel=model->model(sbodyparts,modelIndex);
	tbyte *svertbone=((tbyte*)model->data+smodel->vertinfoindex);
	tbyte *snormbone=((tbyte*)model->data+smodel->norminfoindex);
	vec3_t *sverts=(vec3_t*)(model->data+smodel->vertindex);
	vec3_t *snorms=(vec3_t*)(model->data+smodel->normindex);

	Vector3 normal;
	Vector2 chrome;
	Vector3 *verts=&mTransformedVerts[0];
	Vector3 *norms=&mTransformedNorms[0];
	Vector2 *chromes=&mTransformedChromes[0];
	Vector3 *boneTranslates=&mBoneTranslates[0];
	Quaternion *boneRotates=&mBoneRotates[0];

	for(i=0;i<smodel->numverts;++i){
		Math::mul(verts[i],boneRotates[svertbone[i]],sverts[i]);
		Math::add(verts[i],boneTranslates[svertbone[i]]);
	}

	for(i=0;i<smodel->numnorms;++i){
		Math::mul(norms[i],boneRotates[snormbone[i]],snorms[i]);
		if((model->bone(snormbone[i])->flags&STUDIO_HAS_CHROME)!=0){
			findChrome(chromes[i],norms[i],mChromeForward,mChromeRight);
		}
	}

	VertexBufferAccessor vba(mVertexBuffer);

	for(i=0;i<smodel->nummesh;++i){
		studiomesh *smesh=model->mesh(smodel,i);
		StudioModel::meshdata *mdata=model->findmeshdata(bodypartsIndex,modelIndex,i);
		studiotexture *stexture=model->texture(model->skin(smesh->skinref));
		float s=1.0f/(float)stexture->width;
		float t=1.0f/(float)stexture->height;

		int vertexCount=mdata->vertexStart;
		short *tricmds=(short*)(model->data+smesh->triindex);
		while((l=*(tricmds++))!=0){
			if(l<0){
				l=-l;
			}
			for(;l>0;l--,tricmds+=4){
				vba.set3(vertexCount,0,verts[tricmds[0]]);
				vba.set3(vertexCount,1,norms[tricmds[1]]);
				if((stexture->flags&STUDIO_NF_CHROME)!=0){
					vba.set2(vertexCount,2,chromes[tricmds[1]]);
				}
				else{
					vba.set2(vertexCount,2,tricmds[2]*s,tricmds[3]*t);
				}
				vertexCount++;
			}
		}
	}

	vba.unlock();
}

void StudioModelComponent::findChrome(Vector2 &chrome,const Vector3 &normal,const Vector3 &forward,const Vector3 &right){
	Vector3 chromeUp,chromeRight,temp;
	float n=0;

	Math::cross(chromeUp,right,forward);
	Math::normalize(chromeUp);
	Math::cross(chromeRight,temp,chromeUp);
	Math::normalize(chromeRight);

	/// @todo: Rotate chrome vectors by bone transform

	n=Math::dot(normal,chromeRight);
	chrome.x=(n+1.0f)*32.0f;
	n=Math::dot(normal,chromeUp);
	chrome.y=(n+1.0f)*32.0f;
}

void StudioModelComponent::updateSkeleton(){
	int i,j;

	if(mModel==NULL){
		return;
	}

	if(mLink!=NULL && mLinkModel!=mLink->mModel){
		Log::debug(Categories::TOADLET_TADPOLE,"relinking skeleton");

		setLink(mLink);
	}

	int blendSequenceIndex=-1;

	/// @todo: Support blending weighted animations
	AABox bound;
	float totalWeight=0;
	for(i=0;i<mAnimations.size();++i){
		SequenceAnimation *animation=mAnimations[i];
		if(animation->mWeight>0){
			totalWeight+=animation->mWeight;
			studioseqdesc *sseqdesc=mModel->seqdesc(i);
			studioanim *sanim=mModel->anim(sseqdesc);

			if(blendSequenceIndex<0){
				blendSequenceIndex=i;
				if(blendSequenceIndex!=mBlendSequenceIndex){
					mBlendSequenceIndex=blendSequenceIndex;
					for(j=0;j<4;++j){
						setBlender(j,mBlenderValues[j]);
					}
				}
			}

			findBoneTransforms(&mBoneTranslates[0],&mBoneRotates[0],mModel,sseqdesc,sanim,animation);

			AABox box(sseqdesc->bbmin,sseqdesc->bbmax);
			if(i==0){
				bound.set(box);
			}
			else{
				bound.merge(box);
			}
		}
	}
	if(totalWeight==0){
		AABox box(mModel->header->bbmin,mModel->header->bbmax);
		bound.set(box);
		if(mAnimations.size()>0){
			studioseqdesc *sseqdesc=mModel->seqdesc(0);
			AABox box(sseqdesc->bbmin,sseqdesc->bbmax);
			bound.merge(box);
		}
	}
	mBound->set(bound);

	if(totalWeight==0){
		for(i=0;i<mModel->header->numbones;++i){
			studiobone *sbone=mModel->bone(i);
			mBoneTranslates[i].set(sbone->value[0],sbone->value[1],sbone->value[2]);
			Math::setQuaternionFromEulerAngle(mBoneRotates[i],EulerAngle(EulerAngle::EulerOrder_ZYX,sbone->value[5],sbone->value[4],sbone->value[3]));
		}
	}

	if(false && mBlendSequenceIndex>=0){
		studioseqdesc *sseqdesc=mModel->seqdesc(mBlendSequenceIndex);
		studioanim *sanim=mModel->anim(sseqdesc)+mModel->header->numbones;
		SequenceAnimation *animation=mAnimations[mBlendSequenceIndex];

		findBoneTransforms(&mBlendBoneTranslates[0],&mBlendBoneRotates[0],mModel,sseqdesc,sanim,animation);

		for(i=0;i<mModel->header->numbones;++i){
			Quaternion r;
			Math::slerp(r,mBoneRotates[i],mBlendBoneRotates[i],mAdjustedBlenderValues[0]);
			mBoneRotates[i]=r;
		}
	}

	for(i=0;i<mModel->header->numbones;++i){
		studiobone *sbone=mModel->bone(i);
		int link=mBoneLinks[i];
		if(link>=0){
			mBoneTranslates[i].set(mLink->mBoneTranslates[link]);
			mBoneRotates[i].set(mLink->mBoneRotates[link]);
		}
		else{
			if(sbone->parent>=0){
				Math::preMul(mBoneRotates[i],mBoneRotates[sbone->parent]);
				Math::mul(mBoneTranslates[i],mBoneRotates[sbone->parent]);
				Math::add(mBoneTranslates[i],mBoneTranslates[sbone->parent]);
			}
		}
	}

	mMeshDirty=true;
	mSkeletonDirty=false;
}

void StudioModelComponent::findBoneTransforms(Vector3 *translates,Quaternion *rotates,StudioModel *model,studioseqdesc *sseqdesc,studioanim *sanim,SequenceAnimation *animation){
	float t=animation->mValue * sseqdesc->fps;
	int frame=Math::intFloor(t);
	float s=(t-Math::fromInt(frame));

	if(frame<0){
		frame=0;
		s=0.0;
	}
	else{
		if(sseqdesc->numframes<=1){
			frame=0;
		}
		else if(frame>=sseqdesc->numframes-1){
			frame=sseqdesc->numframes-2;
			s=1.0f;
		}
	}

	studiobone *sbone=model->bone(0);
	int i;
	for(i=0;i<model->header->numbones;++i,sbone++,sanim++){
		if((mBoneScopes[i]&animation->mScope)!=0){
			findBoneTranslate(translates[i],frame,s,sbone,sanim);
			findBoneRotate(rotates[i],frame,s,sbone,sanim);
		}
	}
/*
	if((sseqdesc->motiontype&STUDIO_X)>0){
		translates[sseqdesc->motionbone][0]=0;
	}
*/
}

void StudioModelComponent::findBoneTranslate(Vector3 &r,int frame,float s,studiobone *sbone,studioanim *sanim){
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
			r[j]+=mAdjustedControllerValues[sbone->bonecontroller[j]];
		}
	}
}

void StudioModelComponent::findBoneRotate(Quaternion &r,int frame,float s,studiobone *sbone,studioanim *sanim){
	Quaternion q1,q2;
	Vector3 angle1,angle2;
	studioanimvalue *sanimvalue=NULL;
	int j,k;

	for(j=0;j<3;++j){
		angle2[j]=angle1[j]=sbone->value[j+3];
		if(sanim->offset[j+3]!=0){
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
			angle1[j]+=mAdjustedControllerValues[sbone->bonecontroller[j+3]];
			angle2[j]+=mAdjustedControllerValues[sbone->bonecontroller[j+3]];
		}
	}

	Math::setQuaternionFromEulerAngle(q1,EulerAngle(EulerAngle::EulerOrder_ZYX,angle1.z,angle1.y,angle1.x));
	Math::setQuaternionFromEulerAngle(q2,EulerAngle(EulerAngle::EulerOrder_ZYX,angle2.z,angle2.y,angle2.x));
	Math::slerp(r,q1,q2,s);
}

void StudioModelComponent::createSubModels(){
	int i;
	for(i=0;i<mSubModels.size();++i){
		mSubModels[i]->destroy();
	}
	mSubModels.clear();

	if(mBodypartIndex<0 ||mBodypartIndex>=mModel->header->numbodyparts){
		return;
	}
	studiobodyparts *sbodyparts=mModel->bodyparts(mBodypartIndex);
	if(mModelIndex<0 || mModelIndex>=sbodyparts->nummodels){
		return;
	}
	studiomodel *smodel=mModel->model(sbodyparts,mModelIndex);
	for(i=0;i<smodel->nummesh;++i){
		SubModel::ptr subModel=new SubModel(this,mBodypartIndex,mModelIndex,i,mSkinIndex);
		if(mSharedRenderState!=NULL){
			subModel->mMaterial=mEngine->getMaterialManager()->createSharedMaterial(subModel->mMaterial,mSharedRenderState);
		}
		mSubModels.push_back(subModel);
	}
}

void StudioModelComponent::createSkeletonBuffers(){
	int i;

	if(mModel==NULL){
		return;
	}

	int indexCount=0;
	for(i=0;i<mModel->header->numbones;++i){
		studiobone *bone=mModel->bone(i);
		if(bone->parent>=0){
			indexCount+=2;
		}
	}

	VertexBuffer::ptr skeletonVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION,mModel->header->numbones);
	IndexBuffer::ptr skeletonIndexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,indexCount);

	IndexBufferAccessor iba(skeletonIndexBuffer);

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
	mSkeletonVertexData=new VertexData(mSkeletonVertexBuffer);
	mSkeletonIndexData=new IndexData(IndexData::Primitive_LINES,skeletonIndexBuffer);

	mHitBoxVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION,mModel->header->numhitboxes*8);
	mHitBoxVertexData=new VertexData(mHitBoxVertexBuffer);
	mHitBoxIndexData=new IndexData(IndexData::Primitive_LINES,NULL,0,mHitBoxVertexBuffer->getSize());
}

void StudioModelComponent::updateSkeletonBuffers(){
	int i,j;

	VertexBufferAccessor vba(mSkeletonVertexBuffer);
	for(i=0;i<mModel->header->numbones;++i){
		vba.set3(i,0,mBoneTranslates[i]);
	}
	vba.unlock();

	vba.lock(mHitBoxVertexBuffer);
	Vector3 verts[8];
	for(i=0;i<mModel->header->numhitboxes;++i){
		studiobbox *sbbox=mModel->bbox(i);

		AABox(sbbox->bbmin,sbbox->bbmax).getVertexes(verts);
		for(j=0;j<8;++j){
			Math::mul(verts[j],mBoneRotates[sbbox->bone]);
			Math::add(verts[j],mBoneTranslates[sbbox->bone]);
			vba.set3(i*8+j,0,verts[j]);
		}
	}
	vba.unlock();
}

void StudioModelComponent::destroySkeletonBuffers(){
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

}
}
}
