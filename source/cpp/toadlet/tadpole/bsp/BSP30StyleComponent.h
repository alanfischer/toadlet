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

#ifndef TOADLET_TADPOLE_BSP_BSP30STYLECOMPONENT_H
#define TOADLET_TADPOLE_BSP_BSP30STYLECOMPONENT_H

#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/bsp/BSP30Node.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30StyleComponent:public BaseComponent,public Animatable,public Animation{
public:
	TOADLET_COMPONENT(BSP30StyleComponent);

	BSP30StyleComponent();

	void parentChanged(Node *node);

	bool getActive() const{return false;}

	void setNode(BSP30Node *node);
	BSP30Node *getNode() const{return mNode;}
	
	void setStyle(int style);
	int getStyle() const{return mStyle;}

	void setIntensity(scalar intensity);
	scalar getIntensity() const{return mIntensity;}
	
	// Animatable
	int getNumAnimations(){return 1;}
	Animation *getAnimation(const String &name){return this;}
	Animation *getAnimation(int index){return this;}

	// Animation
	const String &getName() const{return BaseComponent::getName();}

	void setValue(scalar value){setIntensity(value);}
	scalar getValue() const{return getIntensity();}
	scalar getMinValue() const{return 0;}
	scalar getMaxValue() const{return 1;}

	void setWeight(scalar weight){}
	scalar getWeight() const{return Math::ONE;}

protected:
	void update();

	BSP30Node::ptr mNode;
	int mStyle;
	scalar mIntensity;
};

}
}
}

#endif

