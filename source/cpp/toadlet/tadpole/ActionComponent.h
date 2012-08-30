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

#ifndef TOADLET_TADPOLE_ACTIONCOMPONENT_H
#define TOADLET_TADPOLE_ACTIONCOMPONENT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/Action.h>
#include <toadlet/tadpole/BaseComponent.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API ActionComponent:public BaseComponent,public ActionListener{
public:
	TOADLET_OBJECT(ActionComponent);

	ActionComponent(const String &name,Action *action=NULL);
	virtual ~ActionComponent();

	virtual bool parentChanged(Node *node);
	
	virtual void attach(Action *action);
	virtual void remove(Action *action);

	virtual void start();
	virtual void stop();

	virtual void frameUpdate(int dt,int scope);

	virtual bool getActive() const;

	virtual void actionStarted(Action *component);
	virtual void actionStopped(Action *component);

protected:
	Collection<Action::ptr> mActions;
};

}
}

#endif
