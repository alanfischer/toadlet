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
#include <toadlet/tadpole/studio/StudioModelNode.h>
#include <toadlet/tadpole/studio/StudioModelController.h>
#include <toadlet/tadpole/studio/StudioStreamer.h>

namespace toadlet{
namespace tadpole{
namespace studio{

TOADLET_NODE_IMPLEMENT(StudioModelNode,Categories::TOADLET_TADPOLE_STUDIO+".StudioModelNode");

StudioModelNode::SubModel::SubModel(StudioModelNode *modelNode,int bodypartIndex,int modelIndex,int meshIndex,int skinIndex){
	this->modelNode=modelNode;
	this->bodypartIndex=bodypartIndex;
	this->modelIndex=modelIndex;
	this->meshIndex=meshIndex;

	model=modelNode->getModel();
	sbodyparts=model->bodyparts(bodypartIndex);
	smodel=model->model(sbodyparts,modelIndex);
	smesh=model->mesh(smodel,meshIndex);
	mdata=model->findmeshdata(bodypartIndex,modelIndex,meshIndex);
	material=model->materials[model->skin(model->header->numskinref*skinIndex+smesh->skinref)];
	material->retain();
}

void StudioModelNode::SubModel::render(SceneRenderer *renderer) const{
	int i;
	for(i=0;i<mdata->indexDatas.size();++i){
		renderer->getDevice()->renderPrimitive(modelNode->mVertexData,mdata->indexDatas[i]);
	}
}

StudioModelNode::StudioModelNode():super(),
	mRendered(false),
	//mModel,
	//mSubModels,
	//mSharedRenderState,

	mBodypartIndex(0),
	mModelIndex(0),
	mSkinIndex(0),
	mSequenceIndex(0),
	mSequenceTime(0),
	mGaitSequenceIndex(0),
	mGaitSequenceTime(0)
	//mControllerValues[4],mAdjustedControllerValues[4],
	//mBlenderValues[4],mAdjustedBlenderValues[4],
	//mLink,
	//mLinkModel,

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
}

Node *StudioModelNode::create(Scene *scene){
	super::create(scene);

	mRendered=true;

	return this;
}

void StudioModelNode::destroy(){
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

	super::destroy();
}

Node *StudioModelNode::set(Node *node){
	super::set(node);

	StudioModelNode *modelNode=(StudioModelNode*)node;

	setModel(modelNode->getModel());
	setSequence(modelNode->getSequence());
	setSequenceTime(modelNode->getSequenceTime());
	setGaitSequence(modelNode->getGaitSequence());
	setGaitSequenceTime(modelNode->getGaitSequenceTime());
	setRenderSkeleton(modelNode->getRenderSkeleton());
	int i;
	for(i=0;i<4;++i){
		setController(i,modelNode->getController(i));
		setBlender(i,modelNode->getBlender(i));
	}
	setBodypart(modelNode->getBodypart());
	setBodypartModel(modelNode->getBodypartModel());
	setSkin(modelNode->getSkin());

	return this;
}

void *StudioModelNode::hasInterface(int type){
	switch(type){
		case InterfaceType_ATTACHABLE:
			return (Attachable*)this;
		case InterfaceType_DETAILTRACEABLE:
			return (DetailTraceable*)this;
		case InterfaceType_VISIBLE:
			return (Visible*)this;
		default:
			return NULL;
	}
}

void StudioModelNode::logicUpdate(int dt,int scope){
	super::logicUpdate(dt,scope);

	updateSkeleton();

	activate();
}

void StudioModelNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	updateSkeleton();
}

void StudioModelNode::setModel(const String &name){
	Stream::ptr stream=mEngine->openStream(name);
	if(stream==NULL){
		Error::unknown("Unable to find model");
		return;
	}

	StudioStreamer::ptr streamer=new StudioStreamer(mEngine);
	StudioModel::ptr model=shared_static_cast<StudioModel>(streamer->load(stream,NULL,NULL));
	model->setName(name);
	setModel(model);
}

void StudioModelNode::setModel(StudioModel::ptr model){
	mModel=model;

	if(mModel==NULL){
		return;
	}

	mBoneTranslates.resize(model->header->numbones);
	mBoneRotates.resize(model->header->numbones);
	mBoneLinks.resize(mModel->header->numbones,-1);

	mTransformedVerts.resize(MAXSTUDIOVERTS);
	mTransformedNorms.resize(MAXSTUDIOVERTS);
	mTransformedChromes.resize(MAXSTUDIOVERTS);

	mVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,model->vertexCount);
	mVertexData=VertexData::ptr(new VertexData(mVertexBuffer));

	createSubModels();

	setLink(mLink);
	setRenderSkeleton(mSkeletonMaterial!=NULL);

	updateSkeleton();
}

void StudioModelNode::setSequence(int sequence){
	if(sequence<0 || sequence>=mModel->header->numseq){
		sequence=0;
	}

	mSequenceIndex=sequence;

	// Update Blender on sequence change
	int i;
	for(i=0;i<4;++i){
		setBlender(i,mBlenderValues[i]);
	}
}

void StudioModelNode::setSequenceTime(scalar time){
	mSequenceTime=time;
}

void StudioModelNode::setGaitSequence(int sequence){
	if(sequence<0 || sequence>=mModel->header->numseq){
		sequence=0;
	}

	mGaitSequenceIndex=sequence;
}

void StudioModelNode::setGaitSequenceTime(scalar time){
	mGaitSequenceTime=time;
}

void StudioModelNode::setRenderSkeleton(bool skeleton){
	if(skeleton){
		if(mSkeletonMaterial==NULL){
			mSkeletonMaterial=mEngine->getMaterialManager()->createMaterial();
			mSkeletonMaterial->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
			mSkeletonMaterial->getPass()->setPointState(PointState(true,8,false,0,0,0,0,0));
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

void StudioModelNode::setController(int controller,scalar v){
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

void StudioModelNode::setBlender(int blender,scalar v){
	studioseqdesc *sseqdesc=(studioseqdesc*)mModel->seqdesc(mSequenceIndex);
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

void StudioModelNode::setBodypart(int part){
	mBodypartIndex=part;

	createSubModels();
}

void StudioModelNode::setBodypartModel(int model){
	mModelIndex=model;

	createSubModels();
}

void StudioModelNode::setSkin(int skin){
	if(skin<0 || skin>=mModel->header->numskinfamilies){
		skin=0;
	}

	mSkinIndex=skin;

	createSubModels();
}

void StudioModelNode::setLink(StudioModelNode::ptr link){
	mLink=link;

	setDependsUpon(mLink);

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
/*
StudioModelController *StudioModelNode::getController(){
	if(mController==NULL){
		mController=StudioModelController::ptr(new StudioModelController(this,false));
	}
	return (StudioModelController*)mController.get();
}

StudioModelController *StudioModelNode::getGaitController(){
	if(mGaitController==NULL){
		mGaitController=StudioModelController::ptr(new StudioModelController(this,true));
	}
	return (StudioModelController*)mGaitController.get();
}
*/
void StudioModelNode::traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	Vector3 point,normal;
	Segment localSegment,boxSegment;
	Quaternion invrot;
	Transform transform;
	int i;

	transform.set(position,mWorldTransform.getScale(),mWorldTransform.getRotate());
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

RenderState::ptr StudioModelNode::getSharedRenderState(){
	if(mSharedRenderState==NULL){
		mSharedRenderState=mEngine->getMaterialManager()->createRenderState();
		int i;
		for(i=0;i<mSubModels.size();++i){
			mSubModels[i]->material=mEngine->getMaterialManager()->createSharedMaterial(mSubModels[i]->material,mSharedRenderState);
		}
	}

	return mSharedRenderState;
}

void StudioModelNode::gatherRenderables(CameraNode *camera,RenderableSet *set){
	super::gatherRenderables(camera,set);

	if(mModel==NULL || mRendered==false){
		return;
	}

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
		set->queueRenderable(mSubModels[i]);
	}

	if(mSkeletonMaterial!=NULL){
		updateSkeletonBuffers();
		set->queueRenderable(this);
	}
}

void StudioModelNode::render(SceneRenderer *renderer) const{
	renderer->getDevice()->renderPrimitive(mSkeletonVertexData,mSkeletonIndexData);

	mSkeletonIndexData->primitive=IndexData::Primitive_POINTS;
	renderer->getDevice()->renderPrimitive(mSkeletonVertexData,mSkeletonIndexData);
	mSkeletonIndexData->primitive=IndexData::Primitive_LINES;

	renderer->getDevice()->renderPrimitive(mHitBoxVertexData,mHitBoxIndexData);
}

int StudioModelNode::getAttachmentIndex(const String &name){
	int i;
	for(i=0;i<mModel->header->numattachments;++i){
		if(name.equals(mModel->attachment(i)->name)){
			return i;
		}
	}

	return -1;
}

bool StudioModelNode::getAttachmentTransform(Transform &result,int index){
	if(index<0 || index>mModel->header->numattachments){
		return false;
	}

	studioattachment *attachment=mModel->attachment(index);
	Vector3 translate=mBoneTranslates[attachment->bone];
	Quaternion rotate=mBoneRotates[attachment->bone];

	Math::add(translate,attachment->org);
	result.setTranslate(translate);

	Quaternion attachmentRotate;
	Math::setQuaternionFromAxes(attachmentRotate,attachment->vectors[0],attachment->vectors[1],attachment->vectors[2]); /// @todo: Would it work to use setQuaternionFromMatrix with the vector array as a matrix?
	Math::postMul(rotate,attachmentRotate);
	result.setRotate(rotate);

	return true;
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

void StudioModelNode::findChrome(Vector2 &chrome,const Vector3 &normal,const Vector3 &forward,const Vector3 &right){
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

void StudioModelNode::updateSkeleton(){
	int i;

	if(mModel==NULL){
		return;
	}

	if(mLink!=NULL && mLinkModel!=mLink->mModel){
		Logger::debug(Categories::TOADLET_TADPOLE,"relinking skeleton");

		setLink(mLink);
	}

	/// @todo: Move these to member variables?
	static Collection<Vector3> boneTranslates(mModel->header->numbones);
	static Collection<Quaternion> boneRotates(mModel->header->numbones);

	studioseqdesc *sseqdesc=mModel->seqdesc(mSequenceIndex);
	studioanim *sanim=mModel->anim(sseqdesc);
	findBoneTransforms(mBoneTranslates,mBoneRotates,mModel,sseqdesc,sanim,mSequenceTime);

	if(sseqdesc->numblends>1){
		sanim+=mModel->header->numbones;

		findBoneTransforms(boneTranslates,boneRotates,mModel,sseqdesc,sanim,mSequenceTime);

		for(i=0;i<mModel->header->numbones;++i){
			Quaternion r;
			Math::slerp(r,mBoneRotates[i],boneRotates[i],mAdjustedBlenderValues[0]);
			mBoneRotates[i]=r;
		}
	}

	if(mGaitSequenceIndex>0){
		sseqdesc=mModel->seqdesc(mGaitSequenceIndex);
		sanim=mModel->anim(sseqdesc);

		findBoneTransforms(boneTranslates,boneRotates,mModel,sseqdesc,sanim,mGaitSequenceTime);

		for(i=0;i<mModel->header->numbones;++i){
			if(strcmp(mModel->bone(i)->name,"Bip01 Spine")==0) break;
			mBoneTranslates[i].set(boneTranslates[i]);
			mBoneRotates[i].set(boneRotates[i]);
		}
	}

	AABox bound;
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

		studiobbox *sbox=mModel->bbox(i);
		AABox box(sbox->bbmin,sbox->bbmax);
		Math::mul(box,mBoneRotates[i]);
		Math::add(box,mBoneTranslates[i]);
		if(i==0){
			bound.set(box);
		}
		else{
			bound.merge(box);
		}
	}

	mBound.set(bound);
}

void StudioModelNode::findBoneTransforms(Vector3 *translates,Quaternion *rotates,StudioModel *model,studioseqdesc *sseqdesc,studioanim *sanim,float t){
	int frame=Math::intFloor(t);
	float s=(t-Math::fromInt(frame));

	studiobone *sbone=model->bone(0);
	int i;
	for(i=0;i<model->header->numbones;++i,sbone++,sanim++){
		findBoneTranslate(translates[i],frame,s,sbone,sanim);
		findBoneRotate(rotates[i],frame,s,sbone,sanim);
	}
/*
	if((sseqdesc->motiontype&STUDIO_X)>0){
		translates[sseqdesc->motionbone][0]=0;
	}
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
			r[j]+=mAdjustedControllerValues[sbone->bonecontroller[j]];
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
			angle1[j]+=mAdjustedControllerValues[sbone->bonecontroller[j+3]];
			angle2[j]+=mAdjustedControllerValues[sbone->bonecontroller[j+3]];
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

void StudioModelNode::createSubModels(){
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
		SubModel::ptr subModel(new SubModel(this,mBodypartIndex,mModelIndex,i,mSkinIndex));
		if(mSharedRenderState!=NULL){
			subModel->material=mEngine->getMaterialManager()->createSharedMaterial(subModel->material,mSharedRenderState);
		}
		mSubModels.add(subModel);
	}
}

void StudioModelNode::createSkeletonBuffers(){
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
	mSkeletonVertexData=VertexData::ptr(new VertexData(mSkeletonVertexBuffer));
	mSkeletonIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINES,skeletonIndexBuffer));

	mHitBoxVertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STREAM,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION,mModel->header->numhitboxes*8);
	mHitBoxVertexData=VertexData::ptr(new VertexData(mHitBoxVertexBuffer));
	mHitBoxIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINES,NULL,0,mHitBoxVertexBuffer->getSize()));
}

void StudioModelNode::updateSkeletonBuffers(){
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

void StudioModelNode::destroySkeletonBuffers(){
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
