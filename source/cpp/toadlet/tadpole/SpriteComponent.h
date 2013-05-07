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

#ifndef TOADLET_TADPOLE_SPRITECOMPONENT_H
#define TOADLET_TADPOLE_SPRITECOMPONENT_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Spacial.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/Animatable.h>
#include <toadlet/tadpole/action/Animation.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API SpriteComponent:public BaseComponent,public TransformListener,public Spacial,public Renderable,public Visible,public Animatable,public Animation{
public:
	TOADLET_OBJECT(SpriteComponent);
	
	SpriteComponent(Engine *engine);
	void destroy();

	void parentChanged(Node *node);

	void setMaterial(Material *material,int i=0);
	Material *getMaterial(int i=0) const{return mMaterials.size()>i?mMaterials[i]:NULL;}
	int getNumMaterials() const{return mMaterials.size();}

	void setMaterialIndex(int index){mMaterialIndex=Math::intClamp(0,mMaterials.size()-1,index);}
	int getMaterialIndex() const{return mMaterialIndex;}

	void setAlignment(int alignment);
	int getAlignment() const{return mAlignment;}

	// Spacial
	void setTransform(Transform::ptr transform);
	Transform *getTransform() const{return mTransform;}
	Transform *getWorldTransform() const{return mWorldTransform;}
	Bound *getBound() const{return mBound;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mMaterialIndex<mMaterials.size()?mMaterials[mMaterialIndex]:NULL;}
	Transform *getRenderTransform() const{return mWorldTransform;}
	Bound *getRenderBound() const{return mWorldBound;}
	void render(RenderManager *manager) const;

	// Animatable
	int getNumAnimations(){return 1;}
	Animation *getAnimation(const String &name){return this;}
	Animation *getAnimation(int index){return this;}

	// Animation
	const String &getName() const{return BaseComponent::getName();}

	void setValue(scalar value){setMaterialIndex(Math::toInt(value));}
	scalar getValue() const{return Math::fromInt(getMaterialIndex());}
	scalar getMinValue() const{return 0;}
	scalar getMaxValue() const{return mMaterials.size();}

	void setWeight(scalar weight){}
	scalar getWeight() const{return Math::ONE;}

	void setScope(int scope){}
	int getScope() const{return -1;}

	void setAnimationListener(AnimationListener *listener){}
	AnimationListener *getAnimationListener() const{return NULL;}

protected:
	void updateSprite();

	Engine *mEngine;
	int mAlignment;
	Collection<Material::ptr> mMaterials;
	int mMaterialIndex;
	bool mRendered;
	Transform::ptr mTransform;
	Bound::ptr mBound;
	Transform::ptr mWorldTransform;
	Bound::ptr mWorldBound;
	RenderState::ptr mSharedRenderState;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
};

}
}

#endif

