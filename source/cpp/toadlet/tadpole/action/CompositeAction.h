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

#ifndef TOADLET_TADPOLE_ACTION_COMPOSITEACTION_H
#define TOADLET_TADPOLE_ACTION_COMPOSITEACTION_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/action/Action.h>

namespace toadlet{
namespace tadpole{
namespace action{

class TOADLET_API CompositeAction:public Object,public Action,public ActionListener{
public:
	TOADLET_IOBJECT(CompositeAction);

	enum StopStyle{
		StopStyle_ON_FIRST,
		StopStyle_ON_LAST
	};

	CompositeAction();
	virtual ~CompositeAction();

	void attach(Action *action);
	void remove(Action *action);

	void setStopStyle(StopStyle style){mStopStyle=style;}
	StopStyle getStopStyle() const{return mStopStyle;}

	void setNextAction(Action *action){mNextAction=action;}
	Action *getNextAction() const{return mNextAction;}

	void start();
	void stop();

	void update(int dt);

	bool getActive() const;

	void addActionListener(ActionListener *listener);
	void removeActionListener(ActionListener *listener);

	void actionStarted(Action *action);
	void actionStopped(Action *action);

protected:
	Collection<ActionListener*> mListeners;
	Collection<Action::ptr> mActions;
	StopStyle mStopStyle;
	Action::ptr mNextAction;
	bool mActive;
};

}
}
}

#endif
