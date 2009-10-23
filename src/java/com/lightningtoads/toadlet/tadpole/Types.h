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

#ifndef TOADLET_TADPOLE_TYPES_H
#define TOADLET_TADPOLE_TYPES_H

#include <com/lightningtoads/toadlet/Types.h>

#if defined(TOADLET_FIXED_POINT)
	import com.lightningtoads.toadlet.egg.mathfixed.*;
	import com.lightningtoads.toadlet.egg.mathfixed.Math;
#else
	import com.lightningtoads.toadlet.egg.math.*;
	import com.lightningtoads.toadlet.egg.math.Math;
#endif

#if defined(TOADLET_PROFILE)
	#define TOADLET_PROFILE_BEGINSECTION(x) com.lightningtoads.toadlet.egg.Profile.getInstance().beginSection(x)
	#define TOADLET_PROFILE_ENDSECTION(x) com.lightningtoads.toadlet.egg.Profile.getInstance().endSection(x)
	#define TOADLET_PROFILE_ADDTIMINGS() com.lightningtoads.toadlet.egg.Profile.getInstance().addTimings()
	#define TOADLET_PROFILE_CLEARTIMINGS() com.lightningtoads.toadlet.egg.Profile.getInstance().clearTimings()
#else
	#define TOADLET_PROFILE_BEGINSECTION(x) 
	#define TOADLET_PROFILE_ENDSECTION(x) 
	#define TOADLET_PROFILE_ADDTIMINGS()
	#define TOADLET_PROFILE_CLEARTIMINGS()
#endif

#endif


