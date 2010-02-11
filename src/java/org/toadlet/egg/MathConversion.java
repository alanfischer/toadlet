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

package org.toadlet.egg;

#include <org/toadlet/Types.h>

import org.toadlet.egg.math.*;
import org.toadlet.egg.mathfixed.*;

public final class MathConversion{
#if defined(TOADLET_FIXED_POINT)
	public static float scalarToFloat(scalar s){return org.toadlet.egg.mathfixed.Math.toFloat(s);}

	public static org.toadlet.egg.math.Vector3 scalarToFloat(org.toadlet.egg.math.Vector3 result,org.toadlet.egg.mathfixed.Vector3 src){
		result.x=org.toadlet.egg.mathfixed.Math.toFloat(src.x);
		result.y=org.toadlet.egg.mathfixed.Math.toFloat(src.y);
		result.z=org.toadlet.egg.mathfixed.Math.toFloat(src.z);
		return result;
	}

	public static org.toadlet.egg.math.Matrix4x4 scalarToFloat(org.toadlet.egg.math.Matrix4x4 result,org.toadlet.egg.mathfixed.Matrix4x4 src){
		result.setAt(0,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,0)));
		result.setAt(0,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,1)));
		result.setAt(0,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,2)));
		result.setAt(0,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,3)));
		result.setAt(1,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,0)));
		result.setAt(1,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,1)));
		result.setAt(1,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,2)));
		result.setAt(1,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,3)));
		result.setAt(2,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,0)));
		result.setAt(2,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,1)));
		result.setAt(2,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,2)));
		result.setAt(2,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,3)));
		result.setAt(3,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,0)));
		result.setAt(3,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,1)));
		result.setAt(3,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,2)));
		result.setAt(3,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,3)));
		return result;
	}

	public static scalar floatToScalar(float f){return org.toadlet.egg.mathfixed.Math.fromFloat(f);}

	public static org.toadlet.egg.mathfixed.Vector3 floatToScalar(org.toadlet.egg.mathfixed.Vector3 result,org.toadlet.egg.math.Vector3 src){
		result.x=org.toadlet.egg.mathfixed.Math.fromFloat(src.x);
		result.y=org.toadlet.egg.mathfixed.Math.fromFloat(src.y);
		result.z=org.toadlet.egg.mathfixed.Math.fromFloat(src.z);
		return result;
	}

	public static org.toadlet.egg.mathfixed.Matrix4x4 floatToScalar(org.toadlet.egg.mathfixed.Matrix4x4 result,org.toadlet.egg.math.Matrix4x4 src){
		result.setAt(0,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,0)));
		result.setAt(0,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,1)));
		result.setAt(0,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,2)));
		result.setAt(0,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,3)));
		result.setAt(1,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,0)));
		result.setAt(1,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,1)));
		result.setAt(1,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,2)));
		result.setAt(1,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,3)));
		result.setAt(2,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,0)));
		result.setAt(2,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,1)));
		result.setAt(2,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,2)));
		result.setAt(2,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,3)));
		result.setAt(3,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,0)));
		result.setAt(3,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,1)));
		result.setAt(3,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,2)));
		result.setAt(3,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,3)));
		return result;
	}

	public static fixed scalarToFixed(scalar s){return s;}

	public static org.toadlet.egg.mathfixed.Vector3 scalarToFixed(org.toadlet.egg.mathfixed.Vector3 result,org.toadlet.egg.mathfixed.Vector3 src){result.set(src);return result;}

	public static org.toadlet.egg.mathfixed.Matrix4x4 scalarToFixed(org.toadlet.egg.mathfixed.Matrix4x4 result,org.toadlet.egg.mathfixed.Matrix4x4 src){result.set(src);return result;}
	
	public static scalar fixedToScalar(fixed f){return f;}

	public static org.toadlet.egg.mathfixed.Vector3 fixedToScalar(org.toadlet.egg.mathfixed.Vector3 result,org.toadlet.egg.mathfixed.Vector3 src){result.set(src);return result;}

	public static org.toadlet.egg.mathfixed.Matrix4x4 fixedToScalar(org.toadlet.egg.mathfixed.Matrix4x4 result,org.toadlet.egg.mathfixed.Matrix4x4 src){result.set(src);return result;}
#else
	public static float scalarToFloat(scalar s){return s;}

	public static org.toadlet.egg.math.Vector3 scalarToFloat(org.toadlet.egg.math.Vector3 result,org.toadlet.egg.math.Vector3 src){result.set(src);return result;}

	public static org.toadlet.egg.math.Matrix4x4 scalarToFloat(org.toadlet.egg.math.Matrix4x4 result,org.toadlet.egg.math.Matrix4x4 src){result.set(src);return result;}

	public static scalar floatToScalar(float f){return f;}

	public static org.toadlet.egg.math.Vector3 floatToScalar(org.toadlet.egg.math.Vector3 result,org.toadlet.egg.math.Vector3 src){result.set(src);return result;}

	public static org.toadlet.egg.math.Matrix4x4 floatToScalar(org.toadlet.egg.math.Matrix4x4 result,org.toadlet.egg.math.Matrix4x4 src){result.set(src);return result;}

	public static fixed scalarToFixed(scalar s){return org.toadlet.egg.mathfixed.Math.fromFloat(s);}

	public static org.toadlet.egg.mathfixed.Vector3 scalarToFixed(org.toadlet.egg.mathfixed.Vector3 result,org.toadlet.egg.mathfixed.Vector3 src){
		result.x=org.toadlet.egg.mathfixed.Math.fromFloat(src.x);
		result.y=org.toadlet.egg.mathfixed.Math.fromFloat(src.y);
		result.z=org.toadlet.egg.mathfixed.Math.fromFloat(src.z);
		return result;
	}

	public static org.toadlet.egg.mathfixed.Matrix4x4 scalarToFixed(org.toadlet.egg.mathfixed.Matrix4x4 result,org.toadlet.egg.math.Matrix4x4 src){
		result.setAt(0,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,0)));
		result.setAt(0,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,1)));
		result.setAt(0,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,2)));
		result.setAt(0,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(0,3)));
		result.setAt(1,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,0)));
		result.setAt(1,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,1)));
		result.setAt(1,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,2)));
		result.setAt(1,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(1,3)));
		result.setAt(2,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,0)));
		result.setAt(2,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,1)));
		result.setAt(2,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,2)));
		result.setAt(2,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(2,3)));
		result.setAt(3,0,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,0)));
		result.setAt(3,1,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,1)));
		result.setAt(3,2,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,2)));
		result.setAt(3,3,org.toadlet.egg.mathfixed.Math.fromFloat(src.at(3,3)));
		return result;
	}

	public static scalar fixedToScalar(fixed f){return org.toadlet.egg.mathfixed.Math.toFloat(f);}

	public static org.toadlet.egg.math.Vector3 fixedToScalar(org.toadlet.egg.math.Vector3 result,org.toadlet.egg.mathfixed.Vector3 src){
		result.x=org.toadlet.egg.mathfixed.Math.toFloat(src.x);
		result.y=org.toadlet.egg.mathfixed.Math.toFloat(src.y);
		result.z=org.toadlet.egg.mathfixed.Math.toFloat(src.z);
		return result;
	}

	public static org.toadlet.egg.math.Matrix4x4 fixedToScalar(org.toadlet.egg.math.Matrix4x4 result,org.toadlet.egg.mathfixed.Matrix4x4 src){
		result.setAt(0,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,0)));
		result.setAt(0,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,1)));
		result.setAt(0,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,2)));
		result.setAt(0,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(0,3)));
		result.setAt(1,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,0)));
		result.setAt(1,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,1)));
		result.setAt(1,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,2)));
		result.setAt(1,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(1,3)));
		result.setAt(2,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,0)));
		result.setAt(2,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,1)));
		result.setAt(2,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,2)));
		result.setAt(2,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(2,3)));
		result.setAt(3,0,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,0)));
		result.setAt(3,1,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,1)));
		result.setAt(3,2,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,2)));
		result.setAt(3,3,org.toadlet.egg.mathfixed.Math.toFloat(src.at(3,3)));
		return result;
	}
#endif
}
