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

#include <toadlet/tadpole/sensor/NameSensor.h>
#include <toadlet/tadpole/Scene.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace sensor{

NameSensor::NameSensor(Scene *scene):Sensor(mScene){
	mScene=scene;
}

NameSensor::~NameSensor(){
}

bool NameSensor::sense(SensorResultsListener *results){
	results->sensingBeginning();
	int result=senseNames(mScene->getRoot(),results);
	results->sensingEnding();
	return result!=0;
}

int NameSensor::senseNames(Node *node,SensorResultsListener *results){
	int result=0;
	if(mName.equals(node->getName())){
		if(results->resultFound(node,0)){
			result=1;
		}
		else{
			return -1;
		}
	}

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		int i;
		for(i=0;i<parent->getNumChildren();++i){
			int r=senseNames(parent->getChild(i),results);
			if(r<0){
				return -1;
			}
			else if(r>0){
				result=1;
			}
		}
	}

	return result;
}

}
}
}
