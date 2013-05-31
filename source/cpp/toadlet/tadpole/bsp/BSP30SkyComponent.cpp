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
#include <toadlet/tadpole/bsp/BSP30SkyComponent.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

BSP30SkyComponent::BSP30SkyComponent()
	//mNode,
	//mSkyname
{
}

void BSP30SkyComponent::parentChanged(Node *node){
	BaseComponent::parentChanged(node);
	
	if(mParent!=NULL && mParent->getScene()->getRoot()->getType()==BSP30Node::type()){
		setNode((BSP30Node*)mParent->getScene()->getRoot());
	}
}

void BSP30SkyComponent::setNode(BSP30Node *node){
	mNode=node;
	update();
}

void BSP30SkyComponent::setSkyName(const String &skyName){
	mSkyName=skyName;
	update();
}

void BSP30SkyComponent::update(){
	if(mNode!=NULL){
		mNode->setSkyName(mSkyName);
	}
}

}
}
}
