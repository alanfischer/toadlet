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

#ifndef TOADLET_TADPOLE_BSP_BSP30SKYCOMPONENT_H
#define TOADLET_TADPOLE_BSP_BSP30SKYCOMPONENT_H

#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/bsp/BSP30Node.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30SkyComponent:public BaseComponent{
public:
	TOADLET_COMPONENT(BSP30SkyComponent);

	BSP30SkyComponent();

	void parentChanged(Node *node);

	bool getActive() const{return false;}

	void setNode(BSP30Node *node);
	BSP30Node *getNode() const{return mNode;}
	
	void setSkyName(const String &skyName);
	const String &getSkyName() const{return mSkyName;}
	
protected:
	void update();

	BSP30Node::ptr mNode;
	String mSkyName;
};

}
}
}

#endif

