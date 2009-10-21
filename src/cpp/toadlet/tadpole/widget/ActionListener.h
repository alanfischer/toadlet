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

#ifndef TOADLET_TADPOLE_WIDGET_ACTIONLISTENER_H
#define TOADLET_TADPOLE_WIDGET_ACTIONLISTENER_H

namespace toadlet{
namespace tadpole{
namespace widget{

class Widget;

class ActionListener{
public:
	virtual ~ActionListener(){}

	virtual void action(Widget *widget)=0;
};

template<class Type>
class ActionFunctor:public ActionListener{
public:
	ActionFunctor(Type *obj,void (Type::*func)(void)):mObject(obj),mFunction(func){}

	virtual void action(Widget *widget){(mObject->*mFunction)();}

protected:
	Type *mObject;
	void (Type::*mFunction)(void);
};

}
}
}

#endif

