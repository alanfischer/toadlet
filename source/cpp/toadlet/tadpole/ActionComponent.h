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

	bool parentChanged(Node *node);
	
	void attach(Action *action);
	void remove(Action *action);

	void start();
	void stop();

	void frameUpdate(int dt,int scope);

	bool getActive() const;

	void actionStarted(Action *component);
	void actionStopped(Action *component);

protected:
	Collection<Action::ptr> mActions;
};

}
}

#endif
