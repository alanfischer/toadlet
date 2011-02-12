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

#include <toadlet/tadpole/sensor/PotentiallyVisibleSensor.h>
#include <toadlet/tadpole/Scene.h>

using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{
namespace sensor{

PotentiallyVisibleSensor::PotentiallyVisibleSensor(Scene *scene):Sensor(scene){
}

PotentiallyVisibleSensor::~PotentiallyVisibleSensor(){
}

void PotentiallyVisibleSensor::setPoint(const Vector3 &point){
	mPoint.set(point);
}

bool PotentiallyVisibleSensor::sense(SensorResultsListener *results){
	results->sensingBeginning();
	bool result=mScene->getRoot()->sensePotentiallyVisible(results,mPoint);
	results->sensingEnding();
	return result;
}

}
}
}