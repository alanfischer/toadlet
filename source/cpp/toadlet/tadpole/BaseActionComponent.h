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

#ifndef TOADLET_TADPOLE_BASEACTIONCOMPONENT_H
#define TOADLET_TADPOLE_BASEACTIONCOMPONENT_H

#include <toadlet/egg/Object.h>
#include <toadlet/tadpole/ActionComponent.h>
#include <toadlet/tadpole/ActionListener.h>
#include <toadlet/tadpole/node/Node.h>

namespace toadlet{
namespace tadpole{

class BaseActionComponent:protected Object,public ActionComponent{
public:
	TOADLET_OBJECT(BaseActionComponent);
	
	BaseActionComponent():mListener(NULL){}
	BaseActionComponent(const String &name):mListener(NULL){mName=name;}

	virtual void destroy(){
		if(mParent!=NULL){
			mParent->remove(this);
		}
	}

	virtual void setName(const String &name){mName=name;}
	inline const String &getName() const{return mName;}

	virtual bool parentChanged(Node *node){
		if(node!=NULL && mParent!=NULL){
			mParent->remove(this);
		}

		mParent=node;
		
		if(mParent!=NULL){
			mParent->actionAttached(this);
		}

		return true;
	}
	
	virtual void logicUpdate(int dt,int scope){}
	virtual void frameUpdate(int dt,int scope){}

	virtual bool getActive() const{return true;}

	virtual void setActionListener(ActionListener *listener){mListener=listener;}
	virtual ActionListener *getActionListener() const{return mListener;}

protected:
	String mName;
	Node::ptr mParent;
	ActionListener *mListener;
};

}
}

#endif
