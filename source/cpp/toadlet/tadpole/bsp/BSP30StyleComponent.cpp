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
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/bsp/BSP30StyleComponent.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30StyleComponent::BSP30StyleComponent():
	//mBSPNode,
	mStyle(0),
	mIntensity(0)
{
	mIntensity=Math::ONE;
}

bool BSP30StyleComponent::parentChanged(Node *node){
	BaseComponent::parentChanged(node);
	
	if(mParent!=NULL && mParent->getScene()->getRoot()->getType()==BSP30Node::type()){
		setNode((BSP30Node*)mParent->getScene()->getRoot());
	}

	return true;
}

void BSP30StyleComponent::setNode(BSP30Node *node){
	mNode=node;
	update();
}

void BSP30StyleComponent::setStyle(int style){
	mStyle=style;
	update();
}

void BSP30StyleComponent::setIntensity(scalar intensity){
	mIntensity=intensity;
	update();
}

void BSP30StyleComponent::update(){
	if(mNode!=NULL){
		mNode->setStyleIntensity(mStyle,mIntensity);
	}
}

}
}
}
