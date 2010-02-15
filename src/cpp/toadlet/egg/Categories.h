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

#ifndef TOADLET_EGG_CATEGORIES_H
#define TOADLET_EGG_CATEGORIES_H

#include <toadlet/egg/String.h>

namespace toadlet{
namespace egg{

namespace Categories{
	const static String TOADLET=				"toadlet";
	const static String TOADLET_EGG=			TOADLET+".egg";
	const static String TOADLET_EGG_LOGGER=		TOADLET_EGG+".Logger";
	const static String TOADLET_EGG_NET=		TOADLET_EGG+".net";
	const static String TOADLET_FLICK=			TOADLET+".flick";
	const static String TOADLET_HOP=			TOADLET+".hop";
	const static String TOADLET_KNOT=			TOADLET+".knot";
	const static String TOADLET_PEEPER=			TOADLET+".peeper";
	const static String TOADLET_RIBBIT=			TOADLET+".ribbit";
	const static String TOADLET_TADPOLE=		TOADLET+".tadpole";
	const static String TOADLET_TADPOLE_NODE=	TOADLET_TADPOLE+".node";
	const static String TOADLET_PAD=			TOADLET+".pad";
}

}
}

#endif
