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
#include <toadlet/tadpole/Spacial.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API LightComponent:public BaseComponent,public Spacial{
public:
	TOADLET_OBJECT(LightComponent);

	LightComponent();

	inline void setEnabled(bool enabled){mEnabled=enabled;}
	inline bool getEnabled() const{return mEnabled;}

	void setLightState(const LightState &state);
	inline void getLightState(LightState &state){state.set(mLightState);}

	void setDirection(const Vector3 &direction);
	inline const Vector3 &getDirection(){return mDirection;}

	void parentChanged(Node *node);

	bool getActive() const{return false;}

	// Spacial
	Transform *getTransform() const{return NULL;}
	Bound *getBound() const{return mBound;}
	Transform *getWorldTransform() const{return NULL;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

protected:
	bool mEnabled;
	Vector3 mDirection;
	LightState mLightState;
	Bound::ptr mBound;
	Bound::ptr mWorldBound;
};

}
}

#endif

