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

class TOADLET_API BaseComponent:public Object,public Component{
public:
	TOADLET_OBJECT(BaseComponent);
	
	BaseComponent():mParent(NULL),mRoot(NULL){}
	BaseComponent(const String &name):mParent(NULL),mRoot(NULL){mName=name;}

	virtual void destroy();

	virtual Component *componentThis(){return this;}

	virtual void setName(const String &name){mName=name;}
	inline const String &getName() const{return mName;}

	virtual void parentChanged(Node *node);
	virtual void rootChanged(Node *root);

	virtual Node *getParent(){return mParent;}

	virtual void logicUpdate(int dt,int scope){}
	virtual void frameUpdate(int dt,int scope){}

	virtual bool handleEvent(Event *event){return false;}

	virtual bool getActive() const{return false;}

protected:
	Node *mParent;
	Node *mRoot;
	String mName;
};

}
}

// Help to work around an issue with c++ multiple inheritance issues
#define TOADLET_COMPONENT(Class,BaseClass)\
	TOADLET_OBJECT(Class); \
	toadlet::tadpole::Component *componentThis(){return (BaseClass*)this;} \
	void setName(const String &name){toadlet::tadpole::BaseComponent::setName(name);}\
	inline const String &getName() const{return toadlet::tadpole::BaseComponent::getName();}\
	Node *getParent(){return toadlet::tadpole::BaseComponent::getParent();} \
	bool handleEvent(Event *event){return toadlet::tadpole::BaseComponent::handleEvent(event);}

#endif
