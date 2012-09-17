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

#ifndef TOADLET_EGG_MATHFORMATTING_H
#define TOADLET_EGG_MATHFORMATTING_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/mathfixed/Math.h>

namespace toadlet{
namespace egg{

namespace MathFormatting{
#if defined(TOADLET_FIXED_POINT)
	using namespace toadlet::egg::mathfixed;
#else
	using namespace toadlet::egg::math;
#endif

	TOADLET_API String formatBool(bool b);
	TOADLET_API bool parseBool(const char *string);

	TOADLET_API String formatInt(int i);
	TOADLET_API int parseInt(const char *string);

	TOADLET_API String formatScalar(scalar s);
	TOADLET_API scalar parseScalar(const char *string);

	TOADLET_API String formatVector2(const Vector2 &v,const char *separator=",");
	TOADLET_API Vector2 &parseVector2(Vector2 &r,const char *string);
	inline Vector2 parseVector2(const char *string){Vector2 r;parseVector2(r,string);return r;}

	TOADLET_API String formatVector3(const Vector3 &v,const char *separator=",");
	TOADLET_API Vector3 &parseVector3(Vector3 &r,const char *string);
	inline Vector3 parseVector3(const char *string){Vector3 r;parseVector3(r,string);return r;}

	TOADLET_API String formatVector4(const Vector4 &v,const char *separator=",");
	TOADLET_API Vector4 &parseVector4(Vector4 &r,const char *string);
	inline Vector4 parseVector4(const char *string){Vector4 r;parseVector4(r,string);return r;}

	TOADLET_API String formatEulerAngle(const EulerAngle &e,const char *separator=",",bool degrees=false);
	TOADLET_API EulerAngle &parseEulerAngle(EulerAngle &r,const char *string,EulerAngle::EulerOrder order,bool degrees=false);
	inline EulerAngle parseEulerAngle(const char *string,EulerAngle::EulerOrder order,bool degrees=false){EulerAngle r;parseEulerAngle(r,string,order,degrees);return r;}

	TOADLET_API String formatQuaternion(const Quaternion &q,const char *separator=",");
	TOADLET_API Quaternion &parseQuaternion(Quaternion &r,const char *string);
	inline Quaternion parseQuaternion(const char *string){Quaternion r;parseQuaternion(r,string);return r;}

	TOADLET_API String formatByteColor(const Vector4 &c,const char *separator=",");
	TOADLET_API Vector4 &parseByteColor(Vector4 &r,const char *string);
	inline Vector4 parseByteColor(const char *string){Vector4 r;parseByteColor(r,string);return r;}
}

}
}

#endif
