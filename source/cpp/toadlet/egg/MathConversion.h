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

#ifndef TOADLET_EGG_MATHCONVERSION_H
#define TOADLET_EGG_MATHCONVERSION_H

#include <toadlet/egg/Types.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/mathfixed/Math.h>

namespace toadlet{
namespace egg{

namespace MathConversion{
#if defined(TOADLET_FIXED_POINT)
	inline float scalarToFloat(scalar s){return mathfixed::Math::toFloat(s);}

	inline math::Vector2 &scalarToFloat(math::Vector2 &result,const mathfixed::Vector2 &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		return result;
	}

	inline math::Vector3 &scalarToFloat(math::Vector3 &result,const mathfixed::Vector3 &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		return result;
	}

	inline math::Vector4 &scalarToFloat(math::Vector4 &result,const mathfixed::Vector4 &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		result.w=mathfixed::Math::toFloat(src.w);
		return result;
	}

	inline math::Quaternion &scalarToFloat(math::Quaternion &result,const mathfixed::Quaternion &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		result.w=mathfixed::Math::toFloat(src.w);
		return result;
	}

	inline math::EulerAngle &scalarToFloat(math::EulerAngle &result,const mathfixed::EulerAngle &src){
		result.order=(math::EulerAngle::EulerOrder)src.order;
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		return result;
	}

	inline math::Matrix4x4 &scalarToFloat(math::Matrix4x4 &result,const mathfixed::Matrix4x4 &src){
		result.setAt(0,0,mathfixed::Math::toFloat(src.at(0,0)));
		result.setAt(0,1,mathfixed::Math::toFloat(src.at(0,1)));
		result.setAt(0,2,mathfixed::Math::toFloat(src.at(0,2)));
		result.setAt(0,3,mathfixed::Math::toFloat(src.at(0,3)));
		result.setAt(1,0,mathfixed::Math::toFloat(src.at(1,0)));
		result.setAt(1,1,mathfixed::Math::toFloat(src.at(1,1)));
		result.setAt(1,2,mathfixed::Math::toFloat(src.at(1,2)));
		result.setAt(1,3,mathfixed::Math::toFloat(src.at(1,3)));
		result.setAt(2,0,mathfixed::Math::toFloat(src.at(2,0)));
		result.setAt(2,1,mathfixed::Math::toFloat(src.at(2,1)));
		result.setAt(2,2,mathfixed::Math::toFloat(src.at(2,2)));
		result.setAt(2,3,mathfixed::Math::toFloat(src.at(2,3)));
		result.setAt(3,0,mathfixed::Math::toFloat(src.at(3,0)));
		result.setAt(3,1,mathfixed::Math::toFloat(src.at(3,1)));
		result.setAt(3,2,mathfixed::Math::toFloat(src.at(3,2)));
		result.setAt(3,3,mathfixed::Math::toFloat(src.at(3,3)));
		return result;
	}

	inline scalar floatToScalar(float f){return mathfixed::Math::fromFloat(f);}

	inline mathfixed::Vector2 &floatToScalar(mathfixed::Vector2 &result,const math::Vector2 &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		return result;
	}

	inline mathfixed::Vector3 &floatToScalar(mathfixed::Vector3 &result,const math::Vector3 &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		return result;
	}

	inline mathfixed::Vector4 &floatToScalar(mathfixed::Vector4 &result,const math::Vector4 &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		result.w=mathfixed::Math::fromFloat(src.w);
		return result;
	}

	inline mathfixed::Quaternion &floatToScalar(mathfixed::Quaternion &result,const math::Quaternion &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		result.w=mathfixed::Math::fromFloat(src.w);
		return result;
	}

	inline mathfixed::EulerAngle &floatToScalar(mathfixed::EulerAngle &result,const math::EulerAngle &src){
		result.order=(mathfixed::EulerAngle::EulerOrder)src.order;
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		return result;
	}

	inline mathfixed::Matrix4x4 &floatToScalar(mathfixed::Matrix4x4 &result,const math::Matrix4x4 &src){
		result.setAt(0,0,mathfixed::Math::fromFloat(src.at(0,0)));
		result.setAt(0,1,mathfixed::Math::fromFloat(src.at(0,1)));
		result.setAt(0,2,mathfixed::Math::fromFloat(src.at(0,2)));
		result.setAt(0,3,mathfixed::Math::fromFloat(src.at(0,3)));
		result.setAt(1,0,mathfixed::Math::fromFloat(src.at(1,0)));
		result.setAt(1,1,mathfixed::Math::fromFloat(src.at(1,1)));
		result.setAt(1,2,mathfixed::Math::fromFloat(src.at(1,2)));
		result.setAt(1,3,mathfixed::Math::fromFloat(src.at(1,3)));
		result.setAt(2,0,mathfixed::Math::fromFloat(src.at(2,0)));
		result.setAt(2,1,mathfixed::Math::fromFloat(src.at(2,1)));
		result.setAt(2,2,mathfixed::Math::fromFloat(src.at(2,2)));
		result.setAt(2,3,mathfixed::Math::fromFloat(src.at(2,3)));
		result.setAt(3,0,mathfixed::Math::fromFloat(src.at(3,0)));
		result.setAt(3,1,mathfixed::Math::fromFloat(src.at(3,1)));
		result.setAt(3,2,mathfixed::Math::fromFloat(src.at(3,2)));
		result.setAt(3,3,mathfixed::Math::fromFloat(src.at(3,3)));
		return result;
	}

	inline mathfixed::fixed scalarToFixed(scalar s){return s;}

	inline mathfixed::Vector2 &scalarToFixed(mathfixed::Vector2 &result,const mathfixed::Vector2 &src){result.set(src);return result;}

	inline mathfixed::Vector3 &scalarToFixed(mathfixed::Vector3 &result,const mathfixed::Vector3 &src){result.set(src);return result;}

	inline mathfixed::Vector4 &scalarToFixed(mathfixed::Vector4 &result,const mathfixed::Vector4 &src){result.set(src);return result;}

	inline mathfixed::Quaternion &scalarToFixed(mathfixed::Quaternion &result,const mathfixed::Quaternion &src){result.set(src);return result;}

	inline mathfixed::EulerAngle &scalarToFixed(mathfixed::EulerAngle &result,const mathfixed::EulerAngle &src){result.set(src);return result;}

	inline mathfixed::Matrix4x4 &scalarToFixed(mathfixed::Matrix4x4 &result,const mathfixed::Matrix4x4 &src){result.set(src);return result;}

	inline scalar fixedToScalar(mathfixed::fixed f){return f;}

	inline mathfixed::Vector2 &fixedToScalar(mathfixed::Vector2 &result,const mathfixed::Vector2 &src){result.set(src);return result;}

	inline mathfixed::Vector3 &fixedToScalar(mathfixed::Vector3 &result,const mathfixed::Vector3 &src){result.set(src);return result;}

	inline mathfixed::Vector4 &fixedToScalar(mathfixed::Vector4 &result,const mathfixed::Vector4 &src){result.set(src);return result;}

	inline mathfixed::Quaternion &fixedToScalar(mathfixed::Quaternion &result,const mathfixed::Quaternion &src){result.set(src);return result;}

	inline mathfixed::EulerAngle &fixedToScalar(mathfixed::EulerAngle &result,const mathfixed::EulerAngle &src){result.set(src);return result;}

	inline mathfixed::Matrix4x4 &fixedToScalar(mathfixed::Matrix4x4 &result,const mathfixed::Matrix4x4 &src){result.set(src);return result;}
#else
	inline float scalarToFloat(scalar s){return s;}

	inline math::Vector2 &scalarToFloat(math::Vector2 &result,const math::Vector2 &src){result.set(src);return result;}

	inline math::Vector3 &scalarToFloat(math::Vector3 &result,const math::Vector3 &src){result.set(src);return result;}

	inline math::Vector4 &scalarToFloat(math::Vector4 &result,const math::Vector4 &src){result.set(src);return result;}

	inline math::Quaternion &scalarToFloat(math::Quaternion &result,const math::Quaternion &src){result.set(src);return result;}

	inline math::EulerAngle &scalarToFloat(math::EulerAngle &result,const math::EulerAngle &src){result.set(src);return result;}

	inline math::Matrix4x4 &scalarToFloat(math::Matrix4x4 &result,const math::Matrix4x4 &src){result.set(src);return result;}

	inline scalar floatToScalar(float f){return f;}

	inline math::Vector2 &floatToScalar(math::Vector2 &result,const math::Vector2 &src){result.set(src);return result;}

	inline math::Vector3 &floatToScalar(math::Vector3 &result,const math::Vector3 &src){result.set(src);return result;}

	inline math::Vector4 &floatToScalar(math::Vector4 &result,const math::Vector4 &src){result.set(src);return result;}

	inline math::Quaternion &floatToScalar(math::Quaternion &result,const math::Quaternion &src){result.set(src);return result;}

	inline math::EulerAngle &floatToScalar(math::EulerAngle &result,const math::EulerAngle &src){result.set(src);return result;}

	inline math::Matrix4x4 &floatToScalar(math::Matrix4x4 &result,const math::Matrix4x4 &src){result.set(src);return result;}

	inline mathfixed::fixed scalarToFixed(scalar s){return mathfixed::Math::fromFloat(s);}

	inline mathfixed::Vector2 &scalarToFixed(mathfixed::Vector2 &result,const math::Vector2 &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		return result;
	}

	inline mathfixed::Vector3 &scalarToFixed(mathfixed::Vector3 &result,const math::Vector3 &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		return result;
	}

	inline mathfixed::Vector4 &scalarToFixed(mathfixed::Vector4 &result,const math::Vector4 &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		result.w=mathfixed::Math::fromFloat(src.w);
		return result;
	}

	inline mathfixed::Quaternion &scalarToFixed(mathfixed::Quaternion &result,const math::Quaternion &src){
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		result.w=mathfixed::Math::fromFloat(src.w);
		return result;
	}

	inline mathfixed::EulerAngle &scalarToFixed(mathfixed::EulerAngle &result,const math::EulerAngle &src){
		result.order=(mathfixed::EulerAngle::EulerOrder)src.order;
		result.x=mathfixed::Math::fromFloat(src.x);
		result.y=mathfixed::Math::fromFloat(src.y);
		result.z=mathfixed::Math::fromFloat(src.z);
		return result;
	}

	inline mathfixed::Matrix4x4 &scalarToFixed(mathfixed::Matrix4x4 &result,const math::Matrix4x4 &src){
		result.setAt(0,0,mathfixed::Math::fromFloat(src.at(0,0)));
		result.setAt(0,1,mathfixed::Math::fromFloat(src.at(0,1)));
		result.setAt(0,2,mathfixed::Math::fromFloat(src.at(0,2)));
		result.setAt(0,3,mathfixed::Math::fromFloat(src.at(0,3)));
		result.setAt(1,0,mathfixed::Math::fromFloat(src.at(1,0)));
		result.setAt(1,1,mathfixed::Math::fromFloat(src.at(1,1)));
		result.setAt(1,2,mathfixed::Math::fromFloat(src.at(1,2)));
		result.setAt(1,3,mathfixed::Math::fromFloat(src.at(1,3)));
		result.setAt(2,0,mathfixed::Math::fromFloat(src.at(2,0)));
		result.setAt(2,1,mathfixed::Math::fromFloat(src.at(2,1)));
		result.setAt(2,2,mathfixed::Math::fromFloat(src.at(2,2)));
		result.setAt(2,3,mathfixed::Math::fromFloat(src.at(2,3)));
		result.setAt(3,0,mathfixed::Math::fromFloat(src.at(3,0)));
		result.setAt(3,1,mathfixed::Math::fromFloat(src.at(3,1)));
		result.setAt(3,2,mathfixed::Math::fromFloat(src.at(3,2)));
		result.setAt(3,3,mathfixed::Math::fromFloat(src.at(3,3)));
		return result;
	}

	inline scalar fixedToScalar(mathfixed::fixed f){return mathfixed::Math::toFloat(f);}

	inline math::Vector2 &fixedToScalar(math::Vector2 &result,const mathfixed::Vector2 &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		return result;
	}

	inline math::Vector3 &fixedToScalar(math::Vector3 &result,const mathfixed::Vector3 &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		return result;
	}

	inline math::Vector4 &fixedToScalar(math::Vector4 &result,const mathfixed::Vector4 &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		result.w=mathfixed::Math::toFloat(src.w);
		return result;
	}

	inline math::Quaternion &fixedToScalar(math::Quaternion &result,const mathfixed::Quaternion &src){
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		result.w=mathfixed::Math::toFloat(src.w);
		return result;
	}

	inline math::EulerAngle &fixedToScalar(math::EulerAngle &result,const mathfixed::EulerAngle &src){
		result.order=(math::EulerAngle::EulerOrder)src.order;
		result.x=mathfixed::Math::toFloat(src.x);
		result.y=mathfixed::Math::toFloat(src.y);
		result.z=mathfixed::Math::toFloat(src.z);
		return result;
	}

	inline math::Matrix4x4 &fixedToScalar(math::Matrix4x4 &result,const mathfixed::Matrix4x4 &src){
		result.setAt(0,0,mathfixed::Math::toFloat(src.at(0,0)));
		result.setAt(0,1,mathfixed::Math::toFloat(src.at(0,1)));
		result.setAt(0,2,mathfixed::Math::toFloat(src.at(0,2)));
		result.setAt(0,3,mathfixed::Math::toFloat(src.at(0,3)));
		result.setAt(1,0,mathfixed::Math::toFloat(src.at(1,0)));
		result.setAt(1,1,mathfixed::Math::toFloat(src.at(1,1)));
		result.setAt(1,2,mathfixed::Math::toFloat(src.at(1,2)));
		result.setAt(1,3,mathfixed::Math::toFloat(src.at(1,3)));
		result.setAt(2,0,mathfixed::Math::toFloat(src.at(2,0)));
		result.setAt(2,1,mathfixed::Math::toFloat(src.at(2,1)));
		result.setAt(2,2,mathfixed::Math::toFloat(src.at(2,2)));
		result.setAt(2,3,mathfixed::Math::toFloat(src.at(2,3)));
		result.setAt(3,0,mathfixed::Math::toFloat(src.at(3,0)));
		result.setAt(3,1,mathfixed::Math::toFloat(src.at(3,1)));
		result.setAt(3,2,mathfixed::Math::toFloat(src.at(3,2)));
		result.setAt(3,3,mathfixed::Math::toFloat(src.at(3,3)));
		return result;
	}
#endif
};

}
}

#endif
