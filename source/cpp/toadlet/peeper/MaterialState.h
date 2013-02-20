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

#ifndef TOADLET_PEEPER_MATERIALSTATE_H
#define TOADLET_PEEPER_MATERIALSTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class MaterialState{
public:
	enum ShadeType{
		ShadeType_FLAT,
		ShadeType_GOURAUD,
		ShadeType_PHONG
	};

	enum NormalizeType{
		NormalizeType_NONE,
		NormalizeType_RESCALE,
		NormalizeType_NORMALIZE,
	};

	enum AlphaTest{
		AlphaTest_NEVER,
		AlphaTest_LESS,
		AlphaTest_EQUAL,
		AlphaTest_LEQUAL,
		AlphaTest_GREATER,
		AlphaTest_NOTEQUAL,
		AlphaTest_GEQUAL,
		AlphaTest_ALWAYS,
	};

	MaterialState():
		light(true),
		trackColor(false),
		shade(ShadeType_GOURAUD),
		normalize(NormalizeType_RESCALE),
		ambient(Math::ONE_VECTOR4),
		diffuse(Math::ONE_VECTOR4),
		specular(Math::ONE_VECTOR4),
		shininess(Math::ONE),
		emissive(Math::ZERO_VECTOR4),
		alphaTest(AlphaTest_NEVER),
		alphaCutoff(Math::HALF)
	{}

	MaterialState(const MaterialState &lightEffect){
		set(lightEffect);
	}

	MaterialState(bool light1,bool trackColor1=false,ShadeType shade1=ShadeType_GOURAUD,NormalizeType normalize1=NormalizeType_RESCALE):
		ambient(Math::ONE_VECTOR4),
		diffuse(Math::ONE_VECTOR4),
		specular(Math::ONE_VECTOR4),
		shininess(Math::ONE),
		emissive(Math::ZERO_VECTOR4),
		alphaTest(AlphaTest_NEVER),
		alphaCutoff(Math::HALF)
	{
		set(light1,trackColor1,shade1,normalize1);
	}

	MaterialState(const Vector4 &color,bool light1=true):
		trackColor(false),
		shade(ShadeType_GOURAUD),
		normalize(NormalizeType_RESCALE),
		emissive(Math::ZERO_VECTOR4),
		alphaTest(AlphaTest_NEVER),
		alphaCutoff(Math::HALF)
	{
		set(color,light1);
	}

	MaterialState(const Vector4 &ambient1,const Vector4 &diffuse1,const Vector4 &specular1,scalar shininess):
		light(true),
		trackColor(false),
		shade(ShadeType_GOURAUD),
		normalize(NormalizeType_RESCALE),
		emissive(Math::ZERO_VECTOR4),
		alphaTest(AlphaTest_NEVER),
		alphaCutoff(Math::HALF)
	{
		set(ambient1,diffuse1,specular1,shininess);
	}

	MaterialState(AlphaTest alphaTest1,scalar alphaCutoff1=Math::HALF):
		light(false),
		trackColor(false),
		shade(ShadeType_GOURAUD),
		normalize(NormalizeType_RESCALE),
		ambient(Math::ONE_VECTOR4),
		diffuse(Math::ONE_VECTOR4),
		specular(Math::ONE_VECTOR4),
		shininess(Math::ONE),
		emissive(Math::ZERO_VECTOR4)
	{
		set(alphaTest1,alphaCutoff1);
	}

	MaterialState &set(const MaterialState &state){
		light=state.light;
		trackColor=state.trackColor;
		shade=state.shade;
		normalize=state.normalize;
		ambient.set(state.ambient);
		diffuse.set(state.diffuse);
		specular.set(state.specular);
		shininess=state.shininess;
		emissive.set(state.emissive);
		alphaTest=state.alphaTest;
		alphaCutoff=state.alphaCutoff;
		return *this;
	}

	MaterialState &set(bool light1,bool trackColor1=false,ShadeType shade1=ShadeType_GOURAUD,NormalizeType normalize1=NormalizeType_RESCALE){
		light=light1;
		trackColor=trackColor1;
		shade=shade1;
		normalize=normalize1;
		return *this;
	}

	MaterialState &set(const Vector4 &color,bool light1=true){
		light=light1;
		ambient.set(color);
		diffuse.set(color);
		specular.set(color);
		shininess=Math::ONE;
		trackColor=false;
		return *this;
	}

	MaterialState &set(const Vector4 &ambient1,const Vector4 &diffuse1,const Vector4 &specular1,scalar shininess1){
		light=true;
		ambient.set(ambient1);
		diffuse.set(diffuse1);
		specular.set(specular1);
		shininess=shininess1;
		trackColor=false;
		return *this;
	}

	MaterialState &set(AlphaTest alphaTest1,scalar alphaCutoff1=Math::HALF){
		alphaTest=alphaTest1;
		alphaCutoff=alphaCutoff1;
		return *this;
	}

	inline bool equals(const MaterialState &state) const{
		return
			light==state.light && 
			trackColor==state.trackColor &&
			shade==state.shade &&
			normalize==state.normalize &&
			ambient==state.ambient &&
			diffuse==state.diffuse &&
			specular==state.specular &&
			shininess==state.shininess &&
			emissive==state.emissive &&
			trackColor==state.trackColor &&
			alphaTest==state.alphaTest &&
			alphaCutoff==state.alphaCutoff;
	}

	inline bool operator==(const MaterialState &state) const{
		return equals(state);
	}

	inline bool operator!=(const MaterialState &state) const{
		return !equals(state);
	}

	bool light;
	bool trackColor;
	ShadeType shade;
	NormalizeType normalize;
	Vector4 ambient;
	Vector4 diffuse;
	Vector4 specular;
	scalar shininess;
	Vector4 emissive;
	AlphaTest alphaTest;
	scalar alphaCutoff;
};

}
}

#endif
