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

#include <toadlet/egg/MathFormatting.h>
#include <stdio.h>

namespace toadlet{
namespace egg{
namespace MathFormatting{

String formatBool(bool b){
	return b?"true":"false";
}

bool parseBool(const char *string){
	return String(string).toLower()=="true";
}

String formatInt(int i){
	return String()+i;
}

int parseInt(const char *string){
	return String(string).toInt32();
}

String formatScalar(scalar s){
	#if defined(TOADLET_FIXED_POINT)
		return String()+Math::toFloat(s);
	#else
		return String()+s;
	#endif
}

scalar parseScalar(const char *string){
	#if defined(TOADLET_FIXED_POINT)
		return Math::fromFloat(String(string).toFloat());
	#else
		return String(string).toFloat();
	#endif
}

String formatVector2(const Vector2 &v,const char *separator){
	#if defined(TOADLET_FIXED_POINT)
		return String()+Math::toFloat(v.x)+separator+Math::toFloat(v.y);
	#else
		return String()+v.x+separator+v.y;
	#endif
}

Vector2 &parseVector2(Vector2 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f":"%f %f";
	float x=0,y=0;
	sscanf(string,parse,&x,&y);
	#if defined(TOADLET_FIXED_POINT)
		r.x=Math::fromFloat(x);r.y=Math::fromFloat(y);
	#else
		r.x=x;r.y=y;
	#endif
	return r;
}

String formatVector3(const Vector3 &v,const char *separator){
	#if defined(TOADLET_FIXED_POINT)
		return String()+Math::toFloat(v.x)+separator+Math::toFloat(v.y)+separator+Math::toFloat(v.z);
	#else
		return String()+v.x+separator+v.y+separator+v.z;
	#endif
}

Vector3 &parseVector3(Vector3 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f":"%f %f %f";
	float x=0,y=0,z=0;
	sscanf(string,parse,&x,&y,&z);
	#if defined(TOADLET_FIXED_POINT)
		r.x=Math::fromFloat(x);r.y=Math::fromFloat(y);r.z=Math::fromFloat(z);
	#else
		r.x=x;r.y=y;r.z=z;
	#endif
	return r;
}

String formatVector4(const Vector4 &v,const char *separator){
	#if defined(TOADLET_FIXED_POINT)
		return String()+Math::toFloat(v.x)+separator+Math::toFloat(v.y)+separator+Math::toFloat(v.z)+separator+Math::toFloat(v.w);
	#else
		return String()+v.x+separator+v.y+separator+v.z+separator+v.w;
	#endif
}

Vector4 &parseVector4(Vector4 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f,%f":"%f %f %f %f";
	float x=0,y=0,z=0,w=0;
	sscanf(string,parse,&x,&y,&z,&w);
	#if defined(TOADLET_FIXED_POINT)
		r.x=Math::fromFloat(x);r.y=Math::fromFloat(y);r.z=Math::fromFloat(z);r.w=Math::fromFloat(w);
	#else
		r.x=x;r.y=y;r.z=z;r.w=w;
	#endif
	return r;
}

String formatEulerAngle(const EulerAngle &e,const char *separator,bool degrees){
	if(degrees){
		#if defined(TOADLET_FIXED_POINT)
			return String()+Math::toFloat(Math::radToDeg(e.x))+separator+Math::toFloat(Math::radToDeg(e.y))+separator+Math::toFloat(Math::radToDeg(e.z));
		#else
			return String()+Math::radToDeg(e.x)+separator+Math::radToDeg(e.y)+separator+Math::radToDeg(e.z);
		#endif
	}
	else{
		#if defined(TOADLET_FIXED_POINT)
			return String()+Math::toFloat(e.x)+separator+Math::toFloat(e.y)+separator+Math::toFloat(e.z);
		#else
			return String()+e.x+separator+e.y+separator+e.z;
		#endif
	}
}

EulerAngle &parseEulerAngle(EulerAngle &r,const char *string,bool degrees){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f":"%f %f %f";
	float x=0,y=0,z=0;
	sscanf(string,parse,&x,&y,&z);
	if(degrees){
		#if defined(TOADLET_FIXED_POINT)
			r.x=Math::degToRad(Math::fromFloat(x));r.y=Math::degToRad(Math::fromFloat(y));r.z=Math::degToRad(Math::fromFloat(z));
		#else
			r.x=Math::degToRad(x);r.y=Math::degToRad(y);r.z=Math::degToRad(z);
		#endif
	}
	else{
		#if defined(TOADLET_FIXED_POINT)
			r.x=Math::fromFloat(x);r.y=Math::fromFloat(y);r.z=Math::fromFloat(z);
		#else
			r.x=x;r.y=y;r.z=z;
		#endif
	}
	return r;
}

String formatQuaternion(const Quaternion &q,const char *separator){
	#if defined(TOADLET_FIXED_POINT)
		return String()+Math::toFloat(q.x)+separator+Math::toFloat(q.y)+separator+Math::toFloat(q.z)+separator+Math::toFloat(q.w);
	#else
		return String()+q.x+separator+q.y+separator+q.z+separator+q.w;
	#endif
}

Quaternion &parseQuaternion(Quaternion &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f,%f":"%f %f %f %f";
	float x=0,y=0,z=0,w=0;
	sscanf(string,parse,&x,&y,&z,&w);
	#if defined(TOADLET_FIXED_POINT)
		r.x=Math::fromFloat(x);r.y=Math::fromFloat(y);r.z=Math::fromFloat(z);r.w=Math::fromFloat(w);
	#else
		r.x=x;r.y=y;r.z=z;r.w=w;
	#endif
	return r;
}

String formatByteColor(const Vector4 &c,const char *separator){
	int r=Math::toInt(c.x*255),g=Math::toInt(c.y*255),b=Math::toInt(c.z*255),a=Math::toInt(c.w*255);
	return String()+r+separator+g+separator+b+separator+a;
}

Vector4 &parseByteColor(Vector4 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%d,%d,%d,%d":"%d %d %d %d";
	int R=0,g=0,b=0,a=0;
	sscanf(string,parse,&R,&g,&b,&a);
	r.x=Math::fromInt(R)/255;r.y=Math::fromInt(g)/255;r.z=Math::fromInt(b)/255;r.w=Math::fromInt(a)/255;
	return r;
}

}
}
}
