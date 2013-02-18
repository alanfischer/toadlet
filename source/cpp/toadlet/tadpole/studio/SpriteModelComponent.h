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

#ifndef TOADLET_TADPOLE_STUDIO_SPRITEMODELCOMPONENT_H
#define TOADLET_TADPOLE_STUDIO_SPRITEMODELCOMPONENT_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/Animatable.h>
#include <toadlet/tadpole/animation/Animation.h>
#include <toadlet/tadpole/studio/SpriteModel.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class TOADLET_API SpriteModelComponent:public BaseComponent,public Renderable,public Visible,public Animatable,public Animation{
public:
	TOADLET_OBJECT(SpriteModelComponent);

	SpriteModelComponent(Engine *engine);
	void destroy();

	void parentChanged(Node *node);

	void setModel(const String &name);
	void setModel(SpriteModel *model);
	SpriteModel *getModel() const{return mModel;}

	void setSequenceTime(scalar time){mSequenceTime=time;}
	scalar getSequenceTime() const{return mSequenceTime;}

	bool getActive() const{return false;}
	Bound *getBound() const{return mBound;}

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const;
	Transform *getRenderTransform() const{return mParent->getWorldTransform();}
	Bound *getRenderBound() const{return mParent->getWorldBound();}
	void render(RenderManager *manager) const;

	// Animatable
	int getNumAnimations(){return 1;}
	Animation *getAnimation(const String &name){return this;}
	Animation *getAnimation(int index){return this;}

	// Animation
	const String &getName() const{return BaseComponent::getName();}

	void setValue(scalar value){setSequenceTime(value);}
	scalar getValue() const{return getSequenceTime();}
	scalar getMinValue() const{return 0;}
	scalar getMaxValue() const{return mMaterials.size();}

	void setWeight(scalar weight){}
	scalar getWeight() const{return Math::ONE;}

	void setScope(int scope){}
	int getScope() const{return -1;}

	void setAnimationListener(AnimationListener *listener){}
	AnimationListener *getAnimationListener() const{return NULL;}

protected:
	void createBuffers();
	void updateBuffers();
	void destroyBuffers();

	Engine *mEngine;
	SpriteModel::ptr mModel;
	bool mRendered;
	scalar mSequenceTime;
	Collection<Material::ptr> mMaterials;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
	RenderState::ptr mSharedRenderState;
	Bound::ptr mBound;
};

}
}
}

#endif
