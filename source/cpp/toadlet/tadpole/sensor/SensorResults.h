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

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/sensor/SensorResultsListener.h>

namespace toadlet{
namespace tadpole{
namespace sensor{

/// @todo: Change to have a unified Collection<Node> Interface, and move Collection to Vector?
class TOADLET_API SensorResults:public Object,public SensorResultsListener{
public:
	TOADLET_OBJECT(SensorResults);

	SensorResults();

	void setMaxSize(int size);
	int getMaxSize() const{return mMaxSize;}

	void setScope(int scope){mScope=scope;}
	int getScope() const{return mScope;}

	int size() const{return mResults.size();}
	Node *at(int i){return mResults[i];}

	bool contains(Node *node);

	void sensingBeginning();
	bool resultFound(Node *result,scalar distance);
	void sensingEnding();
	
protected:
	int mMaxSize;
	int mScope;
	Collection<Node*> mResults;
	scalar mMaxDistance;
};

}
}
}

#endif
