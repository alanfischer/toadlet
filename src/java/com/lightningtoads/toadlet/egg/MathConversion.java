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

package com.lightningtoads.toadlet.egg;

#include <com/lightningtoads/toadlet/Types.h>

import com.lightningtoads.toadlet.egg.math.*;
import com.lightningtoads.toadlet.egg.mathfixed.*;

public final class MathConversion{
#if defined(TOADLET_FIXED_POINT)
	public static float scalarToFloat(scalar s){return com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(s);}

	public static com.lightningtoads.toadlet.egg.math.Vector3 scalarToFloat(com.lightningtoads.toadlet.egg.math.Vector3 result,com.lightningtoads.toadlet.egg.mathfixed.Vector3 src){
		result.x=com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.x);
		result.y=com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.y);
		result.z=com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.z);
		return result;
	}

	public static com.lightningtoads.toadlet.egg.math.Matrix4x4 scalarToFloat(com.lightningtoads.toadlet.egg.math.Matrix4x4 result,com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 src){
		result.setAt(0,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,0)));
		result.setAt(0,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,1)));
		result.setAt(0,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,2)));
		result.setAt(0,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,3)));
		result.setAt(1,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,0)));
		result.setAt(1,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,1)));
		result.setAt(1,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,2)));
		result.setAt(1,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,3)));
		result.setAt(2,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,0)));
		result.setAt(2,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,1)));
		result.setAt(2,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,2)));
		result.setAt(2,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,3)));
		result.setAt(3,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,0)));
		result.setAt(3,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,1)));
		result.setAt(3,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,2)));
		result.setAt(3,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,3)));
		return result;
	}

	public static scalar floatToScalar(float f){return com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(f);}

	public static com.lightningtoads.toadlet.egg.mathfixed.Vector3 floatToScalar(com.lightningtoads.toadlet.egg.mathfixed.Vector3 result,com.lightningtoads.toadlet.egg.math.Vector3 src){
		result.x=com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.x);
		result.y=com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.y);
		result.z=com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.z);
		return result;
	}

	public static com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 floatToScalar(com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 result,com.lightningtoads.toadlet.egg.math.Matrix4x4 src){
		result.setAt(0,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,0)));
		result.setAt(0,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,1)));
		result.setAt(0,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,2)));
		result.setAt(0,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,3)));
		result.setAt(1,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,0)));
		result.setAt(1,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,1)));
		result.setAt(1,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,2)));
		result.setAt(1,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,3)));
		result.setAt(2,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,0)));
		result.setAt(2,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,1)));
		result.setAt(2,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,2)));
		result.setAt(2,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,3)));
		result.setAt(3,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,0)));
		result.setAt(3,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,1)));
		result.setAt(3,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,2)));
		result.setAt(3,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,3)));
		return result;
	}

	public static fixed scalarToFixed(scalar s){return s;}

	public static com.lightningtoads.toadlet.egg.mathfixed.Vector3 scalarToFixed(com.lightningtoads.toadlet.egg.mathfixed.Vector3 result,com.lightningtoads.toadlet.egg.mathfixed.Vector3 src){result.set(src);return result;}

	public static com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 scalarToFixed(com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 result,com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 src){result.set(src);return result;}
	
	public static scalar fixedToScalar(fixed f){return f;}

	public static com.lightningtoads.toadlet.egg.mathfixed.Vector3 fixedToScalar(com.lightningtoads.toadlet.egg.mathfixed.Vector3 result,com.lightningtoads.toadlet.egg.mathfixed.Vector3 src){result.set(src);return result;}

	public static com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 fixedToScalar(com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 result,com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 src){result.set(src);return result;}
#else
	public static float scalarToFloat(scalar s){return s;}

	public static com.lightningtoads.toadlet.egg.math.Vector3 scalarToFloat(com.lightningtoads.toadlet.egg.math.Vector3 result,com.lightningtoads.toadlet.egg.math.Vector3 src){result.set(src);return result;}

	public static com.lightningtoads.toadlet.egg.math.Matrix4x4 scalarToFloat(com.lightningtoads.toadlet.egg.math.Matrix4x4 result,com.lightningtoads.toadlet.egg.math.Matrix4x4 src){result.set(src);return result;}

	public static scalar floatToScalar(float f){return f;}

	public static com.lightningtoads.toadlet.egg.math.Vector3 floatToScalar(com.lightningtoads.toadlet.egg.math.Vector3 result,com.lightningtoads.toadlet.egg.math.Vector3 src){result.set(src);return result;}

	public static com.lightningtoads.toadlet.egg.math.Matrix4x4 floatToScalar(com.lightningtoads.toadlet.egg.math.Matrix4x4 result,com.lightningtoads.toadlet.egg.math.Matrix4x4 src){result.set(src);return result;}

	public static fixed scalarToFixed(scalar s){return com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(s);}

	public static com.lightningtoads.toadlet.egg.mathfixed.Vector3 scalarToFixed(com.lightningtoads.toadlet.egg.mathfixed.Vector3 result,com.lightningtoads.toadlet.egg.mathfixed.Vector3 src){
		result.x=com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.x);
		result.y=com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.y);
		result.z=com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.z);
		return result;
	}

	public static com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 scalarToFixed(com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 result,com.lightningtoads.toadlet.egg.math.Matrix4x4 src){
		result.setAt(0,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,0)));
		result.setAt(0,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,1)));
		result.setAt(0,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,2)));
		result.setAt(0,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,3)));
		result.setAt(1,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,0)));
		result.setAt(1,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,1)));
		result.setAt(1,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,2)));
		result.setAt(1,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,3)));
		result.setAt(2,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,0)));
		result.setAt(2,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,1)));
		result.setAt(2,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,2)));
		result.setAt(2,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,3)));
		result.setAt(3,0,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,0)));
		result.setAt(3,1,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,1)));
		result.setAt(3,2,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,2)));
		result.setAt(3,3,com.lightningtoads.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,3)));
		return result;
	}

	public static scalar fixedToScalar(fixed f){return com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(f);}

	public static com.lightningtoads.toadlet.egg.math.Vector3 fixedToScalar(com.lightningtoads.toadlet.egg.math.Vector3 result,com.lightningtoads.toadlet.egg.mathfixed.Vector3 src){
		result.x=com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.x);
		result.y=com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.y);
		result.z=com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.z);
		return result;
	}

	public static com.lightningtoads.toadlet.egg.math.Matrix4x4 fixedToScalar(com.lightningtoads.toadlet.egg.math.Matrix4x4 result,com.lightningtoads.toadlet.egg.mathfixed.Matrix4x4 src){
		result.setAt(0,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,0)));
		result.setAt(0,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,1)));
		result.setAt(0,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,2)));
		result.setAt(0,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(0,3)));
		result.setAt(1,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,0)));
		result.setAt(1,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,1)));
		result.setAt(1,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,2)));
		result.setAt(1,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(1,3)));
		result.setAt(2,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,0)));
		result.setAt(2,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,1)));
		result.setAt(2,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,2)));
		result.setAt(2,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(2,3)));
		result.setAt(3,0,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,0)));
		result.setAt(3,1,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,1)));
		result.setAt(3,2,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,2)));
		result.setAt(3,3,com.lightningtoads.toadlet.egg.mathfixed.Math.toFloat(src.at(3,3)));
		return result;
	}
#endif
}
