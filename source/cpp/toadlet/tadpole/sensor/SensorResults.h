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

#ifndef TOADLET_TADPOLE_SENSOR_SENSORRESULTS_H
#define TOADLET_TADPOLE_SENSOR_SENSORRESULTS_H

#include <toadlet/tadpole/sensor/SensorResultsListener.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{
namespace sensor{

class TOADLET_API SensorResults:public Object,public SensorResultsListener{
public:
	TOADLET_OBJECT(SensorResults);

	typedef PointerIteratorRange<Node> NodeRange;
	typedef NodeRange::iterator iterator;

	SensorResults();

	void setScope(int scope){mScope=scope;}
	int getScope() const{return mScope;}

	void sensingBeginning();
	bool resultFound(Node *result,scalar distance);
	void sensingEnding();
	
	inline const iterator &begin(){return mRange.begin();}
	inline const iterator &end(){return mRange.end();}
	inline bool empty() const{return mRange.empty();}
	inline int size() const{return mNodes.size();}

protected:
	int mScope;
	bool mSensing;
	Collection<Node::ptr> mNodes;
	NodeRange mRange;
};

}
}
}

#endif
