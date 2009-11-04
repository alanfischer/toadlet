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

#include <toadlet/peeper/Light.h>
#include <toadlet/peeper/Colors.h>
#include <toadlet/egg/Extents.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

Light::Light():
	specularColor(Colors::BLACK),
	diffuseColor(Colors::WHITE),
	type(Type_DIRECTION),
	linearAttenuation(0),
	spotCutoff(0),
	#if defined(TOADLET_FIXED_POINT)
		radius(Math::sqrt(Extents::MAX_FIXED>>1)),
	#else
		radius(Math::sqrt(Extents::MAX_FLOAT)),
	#endif
	//position,
	direction(0,0,-Math::ONE)
{
}

}
}
