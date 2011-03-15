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

#ifndef TOADLET_PEEPER_LIGHTEFFECT_H
#define TOADLET_PEEPER_LIGHTEFFECT_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class LightEffect{
public:
	LightEffect():
		ambient(Math::ONE_VECTOR4),
		diffuse(Math::ONE_VECTOR4),
		specular(Math::ONE_VECTOR4),
		shininess(Math::ONE),
		emissive(Math::ZERO_VECTOR4),
		trackColor(false)
	{}

	LightEffect(const LightEffect &lightEffect){
		set(lightEffect);
	}

	LightEffect(const Vector4 &color):
		shininess(Math::ONE),
		emissive(Math::ZERO_VECTOR4),
		trackColor(false)
	{
		set(color);
	}

	LightEffect(const Vector4 &ambient1,const Vector4&diffuse1,const Vector4 &specular1,scalar shininess):
		emissive(Math::ZERO_VECTOR4),
		trackColor(false)
	{
		set(ambient1,diffuse1,specular1,shininess);
	}

	LightEffect(bool trackColor1):
		ambient(Math::ONE_VECTOR4),
		diffuse(Math::ONE_VECTOR4),
		specular(Math::ONE_VECTOR4),
		shininess(Math::ONE),
		emissive(Math::ZERO_VECTOR4),
		trackColor(trackColor1)
	{}

	LightEffect &set(const LightEffect &lightEffect){
		ambient.set(lightEffect.ambient);
		diffuse.set(lightEffect.diffuse);
		specular.set(lightEffect.specular);
		shininess=lightEffect.shininess;
		emissive.set(lightEffect.emissive);
		trackColor=lightEffect.trackColor;
		return *this;
	}

	LightEffect &set(const Vector4 &color){
		ambient.set(color);
		diffuse.set(color);
		specular.set(color);
		return *this;
	}

	LightEffect &set(const Vector4 &ambient1,const Vector4 &diffuse1,const Vector4 &specular1,scalar shininess1){
		ambient.set(ambient1);
		diffuse.set(diffuse1);
		specular.set(specular1);
		shininess=shininess1;
		return *this;
	}

	bool operator==(const LightEffect &lightEffect) const{
		return
			ambient==lightEffect.ambient &&
			diffuse==lightEffect.diffuse &&
			specular==lightEffect.specular &&
			shininess==lightEffect.shininess &&
			emissive==lightEffect.emissive &&
			trackColor==lightEffect.trackColor;
	}

	bool operator!=(const LightEffect &lightEffect) const{
		return
			ambient!=lightEffect.ambient ||
			diffuse!=lightEffect.diffuse ||
			specular!=lightEffect.specular ||
			shininess!=lightEffect.shininess ||
			emissive!=lightEffect.emissive ||
			trackColor!=lightEffect.trackColor;
	}

	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;
	scalar shininess;
	Vector4 emissive;
	bool trackColor;
};

}
}

#endif
