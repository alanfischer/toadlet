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

#ifndef TOADLET_TADPOLE_MATHFORMATTER_H
#define TOADLET_TADPOLE_MATHFORMATTER_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API MathFormatter{
public:
	static String formatInteger(int i);
	static int parseInteger(const char *string);

	static String formatScalar(scalar s);
	static scalar parseScalar(const char *string);

	static String formatVector2(const Vector2 &v,const char *separator=",");
	static Vector2 &parseVector2(Vector2 &r,const char *string);
	static Vector2 parseVector2(const char *string){Vector2 r;parseVector2(r,string);return r;}

	static String formatVector3(const Vector3 &v,const char *separator=",");
	static Vector3 &parseVector3(Vector3 &r,const char *string);
	static Vector3 parseVector3(const char *string){Vector3 r;parseVector3(r,string);return r;}

	static String formatVector4(const Vector4 &v,const char *separator=",");
	static Vector4 &parseVector4(Vector4 &r,const char *string);
	static Vector4 parseVector4(const char *string){Vector4 r;parseVector4(r,string);return r;}

	static String formatQuaternion(const Quaternion &q,const char *separator=",");
	static Quaternion &parseQuaternion(Quaternion &r,const char *string);
	static Quaternion parseQuaternion(const char *string){Quaternion r;parseQuaternion(r,string);return r;}

	static String formatByteColor(const Vector4 &c,const char *separator=",");
	static Vector4 &parseByteColor(Vector4 &r,const char *string);
	static Vector4 parseByteColor(const char *string){Vector4 r;parseByteColor(r,string);return r;}
};

}
}

#endif
