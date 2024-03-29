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

#ifndef TOADLET_TADPOLE_ACTION_BASEANIMATION_H
#define TOADLET_TADPOLE_ACTION_BASEANIMATION_H

#include <toadlet/egg/Object.h>
#include <toadlet/tadpole/action/Animation.h>

namespace toadlet{
namespace tadpole{
namespace action{

class TOADLET_API BaseAnimation:public Object,public Animation{
public:
	TOADLET_IOBJECT(BaseAnimation);

	BaseAnimation();
	virtual ~BaseAnimation();

	const String &getName() const{return mName;}

	void setValue(scalar value){}
	scalar getMinValue() const{return 0;}
	scalar getMaxValue() const{return 0;}
	scalar getValue() const{return 0;}

	void setWeight(scalar weight){}
	scalar getWeight() const{return Math::ONE;}

	void setScope(int scope){}
	int getScope() const{return -1;}

	void setAnimationListener(AnimationListener *listener){mListener=listener;}
	AnimationListener *getAnimationListener() const{return mListener;}

protected:
	String mName;
	AnimationListener *mListener;
};

}
}
}

#endif
