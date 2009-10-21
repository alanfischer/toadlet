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

#include <toadlet/peeper/Colors.h>

namespace toadlet{
namespace peeper{

class LightEffect{
public:
	LightEffect():
		ambient(Colors::WHITE),
		diffuse(Colors::WHITE),
		specular(Colors::WHITE),
		shininess(Math::ONE),
		emissive(Colors::BLACK),
		trackColor(false)
	{}

	LightEffect(const LightEffect &lightEffect){
		set(lightEffect);
	}

	LightEffect(const Color &color):
		shininess(Math::ONE),
		emissive(Colors::BLACK),
		trackColor(false)
	{
		set(color);
	}

	LightEffect(bool trackColor):
		ambient(Colors::WHITE),
		diffuse(Colors::WHITE),
		specular(Colors::WHITE),
		shininess(Math::ONE),
		emissive(Colors::BLACK)
	{
		this->trackColor=trackColor;
	}

	LightEffect &set(const LightEffect &lightEffect){
		ambient.set(lightEffect.ambient);
		diffuse.set(lightEffect.diffuse);
		specular.set(lightEffect.specular);
		shininess=lightEffect.shininess;
		emissive.set(lightEffect.emissive);
		trackColor=lightEffect.trackColor;
		return *this;
	}

	LightEffect &set(const Color &color){
		ambient.set(color);
		diffuse.set(color);
		specular.set(color);
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

	Color ambient;
	Color diffuse;
	Color specular;
	scalar shininess;
	Color emissive;
	bool trackColor;
};

}
}

#endif
