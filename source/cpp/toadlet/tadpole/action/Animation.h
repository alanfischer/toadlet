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

#ifndef TOADLET_TADPOLE_ACTION_ANIMATION_H
#define TOADLET_TADPOLE_ACTION_ANIMATION_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/action/AnimationListener.h>

namespace toadlet{
namespace tadpole{
namespace action{

class Animation:public Interface{
public:
	TOADLET_INTERFACE(Animation);

	virtual ~Animation(){}

	virtual const String &getName() const=0;

	virtual void setValue(scalar value)=0;
	virtual scalar getMinValue() const=0;
	virtual scalar getMaxValue() const=0;
	virtual scalar getValue() const=0;

	virtual void setWeight(scalar weight)=0;
	virtual scalar getWeight() const=0;

	virtual void setScope(int scope)=0;
	virtual int getScope() const=0;

	virtual void setAnimationListener(AnimationListener *listener)=0;
};

}
}
}

#endif
