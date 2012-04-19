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

#ifndef TOADLET_TADPOLE_BASECOMPONENT_H
#define TOADLET_TADPOLE_BASECOMPONENT_H

#include <toadlet/egg/Object.h>
#include <toadlet/tadpole/Component.h>

namespace toadlet{
namespace tadpole{

class BaseComponent:public Object,public Component{
public:
	TOADLET_OBJECT(BaseComponent);
	
	BaseComponent(){}
	BaseComponent(const String &name){mName=name;}

	virtual void destroy(){}

	virtual void setName(const String &name){mName=name;}
	inline const String &getName() const{return mName;}

	virtual bool parentChanged(Node *node){return true;}

	virtual void logicUpdate(int dt,int scope){}
	virtual void frameUpdate(int dt,int scope){}

	virtual bool getActive() const{return true;}

protected:
	String mName;
};

}
}

#endif
