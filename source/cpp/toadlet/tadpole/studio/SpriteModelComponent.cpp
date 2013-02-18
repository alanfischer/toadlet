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
#include <toadlet/tadpole/RenderManager.h>
#include <toadlet/tadpole/studio/SpriteModelComponent.h>

namespace toadlet{
namespace tadpole{
namespace studio{

SpriteModelComponent::SpriteModelComponent(Engine *engine):
	//mModel,
	mRendered(false),
	mSequenceTime(0)
	//mMaterials,
	//mVertexData,
	//mIndexData,
	//mSharedRenderState
{
	mEngine=engine;
	mRendered=true;
	mIndexData=new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4);
	mBound=new Bound();
}

void SpriteModelComponent::destroy(){
	mMaterials.clear();

	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}

	mModel=NULL;

	BaseComponent::destroy();
}

void SpriteModelComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->visibleRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->visibleAttached(this);
	}
}

void SpriteModelComponent::setModel(const String &name){
	setModel(shared_static_cast<SpriteModel>(mEngine->getSpriteModelManager()->find(name)));
}

void SpriteModelComponent::setModel(SpriteModel *model){
	mModel=model;

	if(mModel==NULL){
		return;
	}

	mBound->set(mModel->header->boundingradius);

	mMaterials.clear();

	int i;
	for(i=0;i<mModel->materials.size();++i){
		mMaterials.add(mModel->materials[i]);
		if(mSharedRenderState!=NULL){
			mMaterials[i]=mEngine->getMaterialManager()->createSharedMaterial(mMaterials[i],mSharedRenderState);
		}
	}

	if(mVertexData!=NULL){
		mVertexData->destroy();
	}

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_TEX_COORD,4);
	mVertexData=new VertexData(vertexBuffer);
	{
		VertexBufferAccessor vba(vertexBuffer,Buffer::Access_BIT_WRITE);

		float width=mModel->header->width,height=mModel->header->height;

		vba.set3(0,0, -width/2,-height/2,0);
		vba.set2(0,1, 0,0);

		vba.set3(1,0, width/2,-height/2,0);
		vba.set2(1,1, Math::ONE,0);

		vba.set3(2,0, -width/2,height/2,0);
		vba.set2(2,1, 0,Math::ONE);

		vba.set3(3,0, width/2,height/2,0);
		vba.set2(3,1, Math::ONE,Math::ONE);

		vba.unlock();
	}
}

RenderState::ptr SpriteModelComponent::getSharedRenderState(){
	if(mSharedRenderState==NULL){
		mSharedRenderState=mEngine->getMaterialManager()->createRenderState();
		int i;
		for(i=0;i<mMaterials.size();++i){
			if(i==0){
				mEngine->getMaterialManager()->modifyRenderState(mSharedRenderState,mMaterials[i]->getRenderState());
			}
			mMaterials[i]=mEngine->getMaterialManager()->createSharedMaterial(mMaterials[i],mSharedRenderState);
		}
	}

	return mSharedRenderState;
}

void SpriteModelComponent::gatherRenderables(Camera *camera,RenderableSet *set){
	if(mModel==NULL || mRendered==false){
		return;
	}

	set->queueRenderable(this);
}

Material *SpriteModelComponent::getRenderMaterial() const{
	if(mMaterials.size()==0){
		return NULL;
	}

	int sequenceIndex=Math::toInt(mSequenceTime);
	if(sequenceIndex<0){
		return mMaterials[0];
	}
	if(sequenceIndex>=mMaterials.size()){
		return mMaterials[mMaterials.size()-1];
	}
	else{
		return mMaterials[sequenceIndex];
	}
}

void SpriteModelComponent::render(RenderManager *manager) const{
	manager->getDevice()->renderPrimitive(mVertexData,mIndexData);
}

}
}
}
