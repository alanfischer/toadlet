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
#include <toadlet/egg/mathfixed/Math.h>
#include <toadlet/egg/math/Math.h>

namespace toadlet{
namespace egg{

namespace MathFormatting{
	TOADLET_API String formatBool(bool b);
	TOADLET_API String formatInt(int i);

	TOADLET_API String formatScalar(math::real s);
	TOADLET_API String formatVector2(const math::Vector2 &v,const char *separator=",");
	TOADLET_API String formatVector3(const math::Vector3 &v,const char *separator=",");
	TOADLET_API String formatVector4(const math::Vector4 &v,const char *separator=",");
	TOADLET_API String formatEulerAngle(const math::EulerAngle &e,const char *separator=",",bool degrees=false);
	TOADLET_API String formatQuaternion(const math::Quaternion &q,const char *separator=",");
	TOADLET_API String formatByteColor(const math::Vector4 &c,const char *separator=",");

	TOADLET_API String formatScalar(mathfixed::fixed s);
	TOADLET_API String formatVector2(const mathfixed::Vector2 &v,const char *separator=",");
	TOADLET_API String formatVector3(const mathfixed::Vector3 &v,const char *separator=",");
	TOADLET_API String formatVector4(const mathfixed::Vector4 &v,const char *separator=",");
	TOADLET_API String formatEulerAngle(const mathfixed::EulerAngle &e,const char *separator=",",bool degrees=false);
	TOADLET_API String formatQuaternion(const mathfixed::Quaternion &q,const char *separator=",");
	TOADLET_API String formatByteColor(const mathfixed::Vector4 &c,const char *separator=",");

	TOADLET_API bool parseBool(const char *string);
	TOADLET_API int parseInt(const char *string);

	TOADLET_API math::real parseScalar(math::real &s,const char *string);
	TOADLET_API math::Vector2 &parseVector2(math::Vector2 &r,const char *string);
	TOADLET_API math::Vector2 &parseVector2(math::Vector2 &r,const char *string);
	TOADLET_API math::Vector3 &parseVector3(math::Vector3 &r,const char *string);
	TOADLET_API math::Vector4 &parseVector4(math::Vector4 &r,const char *string);
	TOADLET_API math::EulerAngle &parseEulerAngle(math::EulerAngle &r,const char *string,math::EulerAngle::EulerOrder order,bool degrees=false);
	TOADLET_API math::Quaternion &parseQuaternion(math::Quaternion &r,const char *string);
	TOADLET_API math::Vector4 &parseByteColor(math::Vector4 &r,const char *string);

	TOADLET_API mathfixed::fixed parseScalar(mathfixed::fixed &s,const char *string);
	TOADLET_API mathfixed::Vector2 &parseVector2(mathfixed::Vector2 &r,const char *string);
	TOADLET_API mathfixed::Vector2 &parseVector2(mathfixed::Vector2 &r,const char *string);
	TOADLET_API mathfixed::Vector3 &parseVector3(mathfixed::Vector3 &r,const char *string);
	TOADLET_API mathfixed::Vector4 &parseVector4(mathfixed::Vector4 &r,const char *string);
	TOADLET_API mathfixed::EulerAngle &parseEulerAngle(mathfixed::EulerAngle &r,const char *string,mathfixed::EulerAngle::EulerOrder order,bool degrees=false);
	TOADLET_API mathfixed::Quaternion &parseQuaternion(mathfixed::Quaternion &r,const char *string);
	TOADLET_API mathfixed::Vector4 &parseByteColor(mathfixed::Vector4 &r,const char *string);
}

}
}

#endif
