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
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/RenderManager.h>
#include <toadlet/tadpole/SpriteComponent.h>

namespace toadlet{
namespace tadpole{

SpriteComponent::SpriteComponent(Engine *engine):
	mAlignment(0),
	mRendered(false)
	//mMaterial,
	//mSharedRenderState,
	//mVertexData,
	//mIndexData,
{
	mEngine=engine;
	mBound=new Bound();
	mRendered=true;
	mIndexData=new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4);

	setAlignment(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER);
}

void SpriteComponent::destroy(){
	mMaterial=NULL;

	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}

	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	BaseComponent::destroy();
}

void SpriteComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->visibleRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->visibleAttached(this);
	}
}

void SpriteComponent::setMaterial(Material *material){
	mMaterial=material;

	if(mSharedRenderState!=NULL){
		mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mMaterial,mSharedRenderState);
	}
}

void SpriteComponent::setAlignment(int alignment){
	mAlignment=alignment;

	updateSprite();
}

RenderState::ptr SpriteComponent::getSharedRenderState(){
	if(mSharedRenderState==NULL){
		mSharedRenderState=mEngine->getMaterialManager()->createRenderState();
		if(mMaterial!=NULL){
			mEngine->getMaterialManager()->modifyRenderState(mSharedRenderState,mMaterial->getRenderState());

			mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mMaterial,mSharedRenderState);
		}
	}

	return mSharedRenderState;
}

void SpriteComponent::gatherRenderables(Camera *camera,RenderableSet *set){
	if(mVertexData==NULL || mIndexData==NULL || mRendered==false){
		return;
	}

	set->queueRenderable(this);
}

void SpriteComponent::render(RenderManager *manager) const{
	manager->getDevice()->renderPrimitive(mVertexData,mIndexData);
}

void SpriteComponent::updateSprite(){
	scalar x=0,y=0;
	scalar width=Math::ONE,height=Math::ONE; // If we add in a non-normalized option, then this would be the size in non-normalized space.
	if((mAlignment&Font::Alignment_BIT_HCENTER)>0){
		x=-width/2;
	}
	else if((mAlignment&Font::Alignment_BIT_RIGHT)>0){
		x=-width;
	}
	if((mAlignment&Font::Alignment_BIT_VCENTER)>0){
		y=-height/2;
	}
	else if((mAlignment&Font::Alignment_BIT_TOP)>0){
		y=-height;
	}

	if(mVertexData!=NULL){
		mVertexData->destroy();
	}

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_TEX_COORD,4);
	mVertexData=new VertexData(vertexBuffer);
	{
		VertexBufferAccessor vba(vertexBuffer,Buffer::Access_BIT_WRITE);

		vba.set3(0,0, x,y,0);
		vba.set2(0,1, 0,0);

		vba.set3(1,0, x+width,y,0);
		vba.set2(1,1, Math::ONE,0);

		vba.set3(2,0, x,y+height,0);
		vba.set2(2,1, 0,Math::ONE);

		vba.set3(3,0, x+width,y+height,0);
		vba.set2(3,1, Math::ONE,Math::ONE);

		vba.unlock();
	}

	Vector3 origin((2*x+width)/2,(2*y+height)/2,0);
	scalar radius=Math::sqrt(Math::square(width/2) + Math::square(height/2));
	mBound->set(origin,radius);
}

}
}
