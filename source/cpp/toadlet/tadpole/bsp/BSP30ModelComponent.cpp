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
#include <toadlet/tadpole/bsp/BSP30ModelComponent.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30ModelComponent::SubModel::SubModel(BSP30ModelComponent *parent,BSP30Map *map):
	mParent(NULL),
	mMap(NULL),
	mFaces(NULL)
{
	mParent=parent;
	mMap=map;
}

void BSP30ModelComponent::SubModel::render(RenderManager *manager) const{
	mMap->renderFaces(manager->getDevice(),mFaces);
}

BSP30ModelComponent::BSP30ModelComponent(Engine *engine):
	//mMap,
	mModelIndex(0),
	//mSubModels,
	//mSharedRenderState,
	mRendered(false)
{
	mEngine=engine;
	mBound=new Bound();
	mModelIndex=-1;
	mRendered=true;
}

void BSP30ModelComponent::destroy(){
	int i;
	for(i=0;i<mSubModels.size();++i){
		mSubModels[i]->destroy();
	}
	mSubModels.clear();

	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	BaseComponent::destroy();
}

void BSP30ModelComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->visibleRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->visibleAttached(this);
	}
}

void BSP30ModelComponent::setModel(BSP30Map *map,const String &name){
	// BSP Names are in the form of *X where X is an integer corresponding to the model index
	int index=-1;
	if(name.length()>0 && name[0]=='*'){
		index=name.substr(1,name.length()).toInt32();
	}
	setModel(map,index);
}

void BSP30ModelComponent::setModel(BSP30Map *map,int index){
	if(map==NULL || index<0 || index>=map->nmodels){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"invalid Model");
		return;
	}

	int i;
	for(i=0;i<mSubModels.size();++i){
		mSubModels[i]->destroy();
	}
	mSubModels.clear();

	mMap=map;
	mModelIndex=index;

	bmodel *model=&mMap->models[mModelIndex];

	mBound->set(model->mins,model->maxs);
	for(i=0;i<model->numfaces;++i){
		bface *face=&map->faces[model->firstface+i];
		BSP30Map::facedata *facedata=&map->facedatas[model->firstface+i];
		int miptex=map->texinfos[face->texinfo].miptex;
		int j;
		for(j=mSubModels.size()-1;j>=0;--j){
			if(mSubModels[j]->mMaterial==map->materials[miptex]) break;
		}
		if(j<0){
			SubModel::ptr subModel=new SubModel(this,map);
			subModel->mMaterial=map->materials[miptex];
			facedata->next=NULL;
			subModel->mFaces=facedata;
			if(mSharedRenderState!=NULL){
				subModel->mMaterial=mEngine->getMaterialManager()->createSharedMaterial(subModel->mMaterial,mSharedRenderState);
			}
			mSubModels.add(subModel);
		}
		else{
			facedata->next=mSubModels[j]->mFaces;
			mSubModels[j]->mFaces=facedata;
		}
	}
}

RenderState::ptr BSP30ModelComponent::getSharedRenderState(){
	if(mSharedRenderState==NULL){
		mSharedRenderState=mEngine->getMaterialManager()->createRenderState();
		int i;
		for(i=0;i<mSubModels.size();++i){
			if(i==0){
				mEngine->getMaterialManager()->modifyRenderState(mSharedRenderState,mSubModels[i]->mMaterial->getRenderState());
			}
			mSubModels[i]->mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mSubModels[i]->mMaterial,mSharedRenderState);
		}
	}

	return mSharedRenderState;
}

void BSP30ModelComponent::showPlanarFaces(int plane){
	int i;
	for(i=0;i<mSubModels.size();++i){
		BSP30Map::facedata *face=mSubModels[i]->mFaces;
		while(face!=NULL){
			if(mMap->planes[mMap->faces[face->index].planenum].type!=plane){
				face->visible=false;
			}
			else{
				face->visible=true;
			}
			face=face->next;
		}
	}
}

void BSP30ModelComponent::gatherRenderables(Camera*camera,RenderableSet *set){
	if(mRendered==false){
		return;
	}

	int i;
	bmodel *model=&mMap->models[mModelIndex];
	for(i=0;i<model->numfaces;++i){
		mMap->updateFaceLights(model->firstface+i);
	}
	for(i=0;i<mSubModels.size();++i){
		set->queueRenderable(mSubModels[i]);
	}
}

void BSP30ModelComponent::traceSegment(PhysicsCollision &result,const Vector3 &position,const Segment &segment,const Vector3 &size){
	Segment localSegment=segment;
	Transform transform;

	bool transformed=true;//(getWorldTransform()!=Node::identityTransform());
	if(transformed){
		transform.set(position,mParent->getWorldTransform().getScale(),mParent->getWorldTransform().getRotate());
		transform.inverseTransform(localSegment);
	}

	result.time=Math::ONE;
	localSegment.getEndPoint(result.point);
	if(mMap!=NULL){
		int contents=mMap->modelCollisionTrace(result,mModelIndex,size,localSegment.origin,result.point);
		if(contents!=CONTENTS_EMPTY){
			result.scope|=(-1-contents)<<1;
		}
	}

	if(transformed && result.time<Math::ONE){
		transform.transform(result.point);
		transform.transformNormal(result.normal);
	}
}

}
}
}
