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

using namespace toadlet::egg::math::Math;

namespace toadlet{
namespace egg{
namespace MathFormatting{

String formatBool(bool b){
	return b?"true":"false";
}

String formatInt(int i){
	return String()+i;
}

String formatScalar(math::real s){
	return String()+s;
}

String formatVector2(const math::Vector2 &v,const char *separator){
	return String()+v.x+separator+v.y;
}

String formatVector3(const math::Vector3 &v,const char *separator){
	return String()+v.x+separator+v.y+separator+v.z;
}

String formatVector4(const math::Vector4 &v,const char *separator){
	return String()+v.x+separator+v.y+separator+v.z+separator+v.w;
}

String formatEulerAngle(const math::EulerAngle &e,const char *separator,bool degrees){
	if(degrees){
		return String()+radToDeg(e.x)+separator+radToDeg(e.y)+separator+radToDeg(e.z);
	}
	else{
		return String()+e.x+separator+e.y+separator+e.z;
	}
}

String formatQuaternion(const math::Quaternion &q,const char *separator){
	return String()+q.x+separator+q.y+separator+q.z+separator+q.w;
}

String formatByteColor(const math::Vector4 &c,const char *separator){
	int r=toInt(c.x*255),g=toInt(c.y*255),b=toInt(c.z*255),a=toInt(c.w*255);
	return String()+r+separator+g+separator+b+separator+a;
}


String formatScalar(mathfixed::fixed s){
	return formatScalar(toFloat(s));
}

String formatVector2(const mathfixed::Vector2 &v,const char *separator){
	math::Vector2 rv;
	rv.set(toFloat(v.x),toFloat(v.y));
	return formatVector2(rv,separator);
}

String formatVector3(const mathfixed::Vector3 &v,const char *separator){
	math::Vector3 rv;
	rv.set(toFloat(v.x),toFloat(v.y),toFloat(v.z));
	return formatVector3(rv,separator);
}

String formatVector4(const mathfixed::Vector4 &v,const char *separator){
	math::Vector4 rv;
	rv.set(toFloat(v.x),toFloat(v.y),toFloat(v.z),toFloat(v.w));
	return formatVector4(rv,separator);
}

String formatEulerAngle(const mathfixed::EulerAngle &e,const char *separator,bool degrees){
	math::EulerAngle re;
	re.set((math::EulerAngle::EulerOrder)e.order,toFloat(e.x),toFloat(e.y),toFloat(e.z));
	return formatEulerAngle(re,separator);
}

String formatQuaternion(const mathfixed::Quaternion &q,const char *separator){
	math::Quaternion rq;
	rq.set(toFloat(q.x),toFloat(q.y),toFloat(q.z),toFloat(q.w));
	return formatQuaternion(rq,separator);
}

String formatByteColor(const mathfixed::Vector4 &c,const char *separator){
	math::Vector4 rc;
	rc.set(toFloat(c.x),toFloat(c.y),toFloat(c.z),toFloat(c.w));
	return formatByteColor(rc,separator);
}


bool parseBool(const char *string){
	return String(string).toLower()=="true";
}

int parseInt(const char *string){
	return String(string).toInt32();
}

math::real parseScalar(math::real &s,const char *string){
	s=String(string).toFloat();
	return s;
}

math::Vector2 &parseVector2(math::Vector2 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f":"%f %f";
	float x=0,y=0;
	sscanf(string,parse,&x,&y);
	r.set(x,y);
	return r;
}

math::Vector3 &parseVector3(math::Vector3 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f":"%f %f %f";
	float x=0,y=0,z=0;
	sscanf(string,parse,&x,&y,&z);
	r.set(x,y,z);
	return r;
}

math::Vector4 &parseVector4(math::Vector4 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f,%f":"%f %f %f %f";
	float x=0,y=0,z=0,w=0;
	sscanf(string,parse,&x,&y,&z,&w);
	r.set(x,y,z,w);
	return r;
}

math::EulerAngle &parseEulerAngle(math::EulerAngle &r,const char *string,math::EulerAngle::EulerOrder order,bool degrees){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f":"%f %f %f";
	float x=0,y=0,z=0;
	sscanf(string,parse,&x,&y,&z);
	if(degrees){
		r.set(order,degToRad(x),degToRad(y),degToRad(z));
	}
	else{
		r.set(order,x,y,z);
	}
	return r;
}

math::Quaternion &parseQuaternion(math::Quaternion &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%f,%f,%f,%f":"%f %f %f %f";
	float x=0,y=0,z=0,w=0;
	sscanf(string,parse,&x,&y,&z,&w);
	r.set(x,y,z,w);
	return r;
}

math::Vector4 &parseByteColor(math::Vector4 &r,const char *string){
	const char *parse=strchr(string,',')!=NULL?"%d,%d,%d,%d":"%d %d %d %d";
	int R=0,g=0,b=0,a=0;
	sscanf(string,parse,&R,&g,&b,&a);
	r.set(fromInt(R)/255,fromInt(g)/255,fromInt(b),fromInt(a)/255);
	return r;
}

mathfixed::fixed parseScalar(mathfixed::fixed &s,const char *string){
	math::real rs;
	parseScalar(rs,string);
	s=fromFloat(rs);
	return s;
}

mathfixed::Vector2 &parseVector2(mathfixed::Vector2 &r,const char *string){
	math::Vector2 rv;
	parseVector2(rv,string);
	r.set(fromFloat(rv.x),fromFloat(rv.y));
	return r;
}

mathfixed::Vector3 &parseVector3(mathfixed::Vector3 &r,const char *string){
	math::Vector3 rv;
	parseVector3(rv,string);
	r.set(fromFloat(rv.x),fromFloat(rv.y),fromFloat(rv.z));
	return r;
}

mathfixed::Vector4 &parseVector4(mathfixed::Vector4 &r,const char *string){
	math::Vector4 rv;
	parseVector4(rv,string);
	r.set(fromFloat(rv.x),fromFloat(rv.y),fromFloat(rv.z),fromFloat(rv.w));
	return r;
}

mathfixed::EulerAngle &parseEulerAngle(mathfixed::EulerAngle &r,const char *string,mathfixed::EulerAngle::EulerOrder order,bool degrees){
	math::EulerAngle ra;
	parseEulerAngle(ra,string,(math::EulerAngle::EulerOrder)order,degrees);
	r.set((mathfixed::EulerAngle::EulerOrder)ra.order,fromFloat(ra.x),fromFloat(ra.y),fromFloat(ra.z));
	return r;
}

mathfixed::Quaternion &parseQuaternion(mathfixed::Quaternion &r,const char *string){
	math::Quaternion rq;
	parseQuaternion(rq,string);
	r.set(fromFloat(rq.x),fromFloat(rq.y),fromFloat(rq.z),fromFloat(rq.w));
	return r;
}

mathfixed::Vector4 &parseByteColor(mathfixed::Vector4 &r,const char *string){
	math::Vector4 rv;
	parseByteColor(rv,string);
	r.set(fromFloat(rv.x),fromFloat(rv.y),fromFloat(rv.z),fromFloat(rv.w));
	return r;
}

}
}
}
