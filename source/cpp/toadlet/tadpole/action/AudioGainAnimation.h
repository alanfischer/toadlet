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

#ifndef TOADLET_TADPOLE_ACTION_AUDIOGAINANIMATION_H
#define TOADLET_TADPOLE_ACTION_AUDIOGAINANIMATION_H

#include <toadlet/tadpole/action/BaseAnimation.h>
#include <toadlet/tadpole/AudioComponent.h>
#include <toadlet/tadpole/Sequence.h>

namespace toadlet{
namespace tadpole{
namespace action{

class TOADLET_API AudioGainAnimation:public BaseAnimation{
public:
	TOADLET_OBJECT(AudioGainAnimation);

	AudioGainAnimation(AudioComponent *target,Sequence *sequence,int trackIndex=0);
	virtual ~AudioGainAnimation(){}

	void setTarget(AudioComponent *target);
	inline AudioComponent *getTarget() const{return mTarget;}

	bool setSequence(Sequence *sequence,int trackIndex);
	inline Sequence *getSequence() const{return mSequence;}

	void setValue(scalar value);
	scalar getValue() const{return mValue;}
	scalar getMinValue() const{return 0;}
	scalar getMaxValue() const{return mSequence->getLength();}

protected:
	AudioComponent::ptr mTarget;
	Sequence::ptr mSequence;
	Track::ptr mTrack;
	scalar mValue;
};

}
}
}

#endif
