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

#ifndef TOADLET_TADPOLE_ANIMATION_MATERIALANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_MATERIALANIMATION_H

#include <toadlet/egg/Object.h>
#include <toadlet/tadpole/animation/BaseAnimation.h>
#include <toadlet/tadpole/material/Material.h>
#include <toadlet/tadpole/ColorSequence.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class Controller;

class TOADLET_API MaterialAnimation:public BaseAnimation{
public:
	TOADLET_OBJECT(MaterialAnimation);

	MaterialAnimation(Material *target,ColorSequence *sequence,int trackIndex=0);
	virtual ~MaterialAnimation();

	void setTarget(Material *target);
	inline Material *getTarget() const{return mTarget;}

	void setSequence(ColorSequence *sequence,int trackIndex);
	inline ColorSequence *getSequence() const{return mSequence;}

	void setValue(scalar value);
	scalar getValue() const{return mValue;}
	scalar getMinValue() const{return 0;}
	scalar getMaxValue() const{return mSequence->getLength();}

	void setWeight(scalar weight){}
	scalar getWeight() const{return Math::ONE;}

protected:
	Material::ptr mTarget;
	ColorSequence::ptr mSequence;
	ColorTrack::ptr mTrack;
	scalar mValue;
};

}
}
}

#endif
