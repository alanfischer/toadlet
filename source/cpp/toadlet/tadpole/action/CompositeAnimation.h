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

#ifndef TOADLET_TADPOLE_ACTION_COMPOSITEANIMATION_H
#define TOADLET_TADPOLE_ACTION_COMPOSITEANIMATION_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/action/BaseAnimation.h>

namespace toadlet{
namespace tadpole{
namespace action{

class TOADLET_API CompositeAnimation:public BaseAnimation,public AnimationListener{
public:
	TOADLET_OBJECT(CompositeAnimation);

	CompositeAnimation();
	virtual ~CompositeAnimation();

	void attach(Animation *animation);
	void remove(Animation *animation);

	void setValue(scalar value);
	scalar getValue() const{return mValue;}
	scalar getMinValue() const{return mMinValue;}
	scalar getMaxValue() const{return mMaxValue;}

	void setWeight(scalar weight);
	scalar getWeight() const{return mWeight;}

	void animationExtentsChanged(Animation *animation);

protected:
	Collection<Animation::ptr> mAnimations;
	scalar mValue,mMinValue,mMaxValue;
	scalar mWeight;
};

}
}
}

#endif
