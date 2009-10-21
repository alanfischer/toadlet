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

#ifndef TOADLET_PEEPER_LIGHT_H
#define TOADLET_PEEPER_LIGHT_H

#include <toadlet/egg/WeakPointer.h>
#include <toadlet/peeper/Colors.h>

namespace toadlet{
namespace peeper{

class TOADLET_API Light{
public:
	TOADLET_SHARED_POINTERS(Light,Light);

	enum Type{
		Type_POSITION,
		Type_DIRECTION,
		Type_SPOT,
	};

	Light();

	inline void setSpecularColor(const Color &specularColor){mSpecularColor.set(specularColor);}
	inline const Color &getSpecularColor() const{return mSpecularColor;}

	inline void setDiffuseColor(const Color &diffuseColor){mDiffuseColor.set(diffuseColor);}
	inline const Color &getDiffuseColor() const{return mDiffuseColor;}

	inline void setType(Type type){mType=type;}
	inline Type getType() const{return mType;}

	inline void setLinearAttenuation(scalar attenuation){mLinearAttenuation=attenuation;}
	inline scalar getLinearAttenuation() const{return mLinearAttenuation;}

	inline void setSpotCutoff(scalar cutoff){mSpotCutoff=cutoff;}
	inline scalar getSpotCutoff() const{return mSpotCutoff;}

	inline void setRadius(scalar radius){mRadius=radius;}
	inline scalar getRadius() const{return mRadius;}

	inline void setDirection(const Vector3 &direction){mDirection.set(direction);}
	inline const Vector3 &getDirection() const{return mDirection;}

	inline void setPosition(const Vector3 &position){mPosition.set(position);}
	inline const Vector3 &getPosition() const{return mPosition;}

protected:
	Color mSpecularColor;
	Color mDiffuseColor;
	Type mType;
	scalar mLinearAttenuation;
	scalar mSpotCutoff;
	scalar mRadius;
	Vector3 mPosition;
	Vector3 mDirection;
};

}
}

#endif
