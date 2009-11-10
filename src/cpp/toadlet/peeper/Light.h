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
	TOADLET_SHARED_POINTERS(Light);

	enum Type{
		Type_POSITION,
		Type_DIRECTION,
		Type_SPOT,
	};

	Light();

	inline void setSpecularColor(const Color &color){specularColor.set(color);}
	inline const Color &getSpecularColor() const{return specularColor;}

	inline void setDiffuseColor(const Color &color){diffuseColor.set(color);}
	inline const Color &getDiffuseColor() const{return diffuseColor;}

	inline void setType(Type type1){type=type1;}
	inline Type getType() const{return type;}

	inline void setLinearAttenuation(scalar attenuation){linearAttenuation=attenuation;}
	inline scalar getLinearAttenuation() const{return linearAttenuation;}

	inline void setSpotCutoff(scalar cutoff){spotCutoff=cutoff;}
	inline scalar getSpotCutoff() const{return spotCutoff;}

	inline void setRadius(scalar radius1){radius=radius1;}
	inline scalar getRadius() const{return radius;}

	inline void setDirection(const Vector3 &direction1){direction.set(direction1);}
	inline const Vector3 &getDirection() const{return direction;}

	inline void setPosition(const Vector3 &position1){position.set(position1);}
	inline const Vector3 &getPosition() const{return position;}

	Color specularColor;
	Color diffuseColor;
	Type type;
	scalar linearAttenuation;
	scalar spotCutoff;
	scalar radius;
	Vector3 position;
	Vector3 direction;
};

}
}

#endif
