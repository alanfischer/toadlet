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

#ifndef TOADLET_TADPOLE_LIGHTCOMPONENT_H
#define TOADLET_TADPOLE_LIGHTCOMPONENT_H

#include <toadlet/peeper/LightState.h>
#include <toadlet/tadpole/BaseComponent.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API LightComponent:public BaseComponent{
public:
	TOADLET_OBJECT(LightComponent);

	LightComponent();

	inline void setEnabled(bool enabled){mEnabled=enabled;}
	inline bool getEnabled() const{return mEnabled;}

	inline void setLightState(const LightState &state){mDirection.set(state.direction);mLightState.set(state);}
	inline void getLightState(LightState &state){state.set(mLightState);}

	inline void setDirection(const Vector3 &direction){mDirection.set(direction);mLightState.direction.set(direction);}
	inline const Vector3 &getDirection(){return mDirection;}

	void parentChanged(Node *node);

	void frameUpdate(int dt,int scope);

	bool getActive() const{return false;}
	Bound *getBound() const{return mBound;}

protected:
	bool mEnabled;
	Vector3 mDirection;
	LightState mLightState;
	Bound::ptr mBound;
};

}
}

#endif

