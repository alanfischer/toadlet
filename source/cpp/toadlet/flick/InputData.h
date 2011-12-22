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

#ifndef TOADLET_FLICK_INPUTDATA_H
#define TOADLET_FLICK_INPUTDATA_H

#include <toadlet/egg/Collection.h>
#include <toadlet/flick/Types.h>

namespace toadlet{
namespace flick{

class InputData{
public:	
	enum Semantic{
		Semantic_JOY_BUTTON_PRESSED=0,
		Semantic_JOY_BUTTON_RELEASED,
		Semantic_JOY_DIRECTION,
		Semantic_JOY_ROTATION,
		Semantic_MAX_JOY,
		
		Semantic_MOTION_ACCELERATION=0,
		Semantic_MOTION_VELOCITY,
		Semantic_MAX_MOTION,

		Semantic_LIGHT=0,
		Semantic_MAX_LIGHT,

		Semantic_PROXIMITY=0,
		Semantic_MAX_PROXIMITY,
	};

	InputData(int type1=0,int time1=0,int size=0):
		type(type1),
		time(time1),
		valid(0)
	{
		values.resize(size);
	}

	void set(const InputData &data){
		type=data.type;
		time=data.time;
		valid=data.valid;
		values=data.values;
	}
	
	int type;
	uint64 time;
	int valid;
	Collection<Vector4> values;
};

}
}

#endif
