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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/RenderManager.h>
#include <toadlet/tadpole/SpriteComponent.h>

namespace toadlet{
namespace tadpole{

SpriteComponent::SpriteComponent(Engine *engine):
	mAlignment(0),
	mNormalized(true),
	//mMaterials,
	mMaterialIndex(0),
	mRendered(true)
	//mSharedRenderState,
	//mVertexData,
	//mIndexData,
{
	mEngine=engine;
	mTransform=new Transform();
	mTransform->setTransformListener(this);
	mBound=new Bound();
	mWorldTransform=new Transform();
	mWorldBound=new Bound();

	mIndexData=new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4);

	setAlignment(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER);
}

void SpriteComponent::destroy(){
	mMaterials.clear();

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

void SpriteComponent::setMaterial(const String &name){
	mEngine->getMaterialManager()->find(name,this);
}

void SpriteComponent::setMaterial(Material *material,int i){
	if(mMaterials.size()==i+1 && material==NULL){
		mMaterials.resize(i);
		return;
	}

	if(mMaterials.size()<=i){
		mMaterials.resize(i+1);
	}
	mMaterials[i]=material;

	if(mSharedRenderState!=NULL){
		if(mMaterials[i]!=NULL){
			mMaterials[i]=mEngine->getMaterialManager()->createSharedMaterial(mMaterials[i],mSharedRenderState);
		}
	}

	if(i==0){
		updateSprite();
	}
}

void SpriteComponent::setAlignment(int alignment){
	mAlignment=alignment;

	updateSprite();
}

void SpriteComponent::setNormalized(bool normalize){
	mNormalized=normalize;

	updateSprite();
}

void SpriteComponent::setTransform(Transform::ptr transform){
	if(transform==NULL){
		mTransform->reset();
	}
	else{
		mTransform->set(transform);
	}
}

void SpriteComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	mWorldTransform->setTransform(mParent->getWorldTransform(),mTransform);
	mWorldBound->transform(mBound,mWorldTransform);

	if(transform==mTransform){
		mParent->boundChanged();
	}
}

void SpriteComponent::setSharedRenderState(RenderState::ptr renderState){
	int i;
	if(renderState==NULL){
		renderState=mEngine->getMaterialManager()->createRenderState();
		for(i=0;i<mMaterials.size();++i){
			if(mMaterials[i]!=NULL){
				mEngine->getMaterialManager()->modifyRenderState(renderState,mMaterials[i]->getRenderState());
			}
		}
	}

	mSharedRenderState=renderState;
	for(i=0;i<mMaterials.size();++i){
		if(mMaterials[i]!=NULL){
			mMaterials[i]=mEngine->getMaterialManager()->createSharedMaterial(mMaterials[i],mSharedRenderState);
		}
	}
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
	scalar width=Math::ONE,height=Math::ONE;

	if(mNormalized==false && mMaterials.size()>0){
		Material *material=mMaterials[0];
		Texture *texture=material->getPass()->getTexture(Shader::ShaderType_VERTEX,0);
		if(texture==NULL){
			texture=material->getPass()->getTexture(Shader::ShaderType_FRAGMENT,0);
		}
		if(texture!=NULL){
			width=Math::fromInt(texture->getFormat()->getWidth());
			height=Math::fromInt(texture->getFormat()->getHeight());
		}
	}

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
