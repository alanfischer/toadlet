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

#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace egg{
namespace math{

bool Math::setEulerAngleXYZFromMatrix3x3(EulerAngle &r,const Matrix3x3 &m,real epsilon){
	if(m.at(1,0)>1-epsilon){ // North Pole singularity
		r.x=atan2(m.at(0,2),m.at(2,2));
		r.y=HALF_PI;
		r.z=0;
		return false;
	}
	else if(m.at(1,0)<-(1-epsilon)){ // South Pole singularity
		r.x=atan2(m.at(0,2),m.at(2,2));
		r.y=-HALF_PI;
		r.z=0;
		return false;
	}
	else{
		r.x=atan2(-m.at(2,0),m.at(0,0));
		r.y=asin(m.at(1,0));
		r.z=atan2(-m.at(1,2),m.at(1,1));
		return true;
	}
}

bool Math::setEulerAngleXYZFromQuaternion(EulerAngle &r,const Quaternion &q,real epsilon){
	real test=q.x*q.y + q.z*q.w;
	if(test>0.5-epsilon){ // North Pole singularity
		r.x=2*atan2(q.x,q.w);
		r.y=HALF_PI;
		r.z=0;
		return false;
	}
	else if(test<-(0.5-epsilon)){ // South Pole singularity
		r.x=-2*atan2(q.x,q.w);
		r.y=-HALF_PI;
		r.z=0;
		return false;
	}
	else{
		real sqx=q.x*q.x;
		real sqy=q.y*q.y;
		real sqz=q.z*q.z;
		r.x=atan2(2*q.y*q.w-2*q.x*q.z, 1 - 2*sqy - 2*sqz);
		r.y=asin(2*test);
		r.z=atan2(2*q.x*q.w-2*q.y*q.z, 1 - 2*sqx - 2*sqz);
		return true;
	}
}

void Math::transpose(Matrix3x3 &r,const Matrix3x3 &m){
	r.data[0+0*3]=m.data[0+0*3]; r.data[0+1*3]=m.data[1+0*3]; r.data[0+2*3]=m.data[2+0*3];
	r.data[1+0*3]=m.data[0+1*3]; r.data[1+1*3]=m.data[1+1*3]; r.data[1+2*3]=m.data[2+1*3];
	r.data[2+0*3]=m.data[0+2*3]; r.data[2+1*3]=m.data[1+2*3]; r.data[2+2*3]=m.data[2+2*3];
}

real Math::determinant(const Matrix3x3 &m){
	return
		-m.data[0+0*3]*m.data[1+1*3]*m.data[2+2*3] + m.data[0+0*3]*m.data[2+1*3]*m.data[1+2*3] +
		 m.data[0+1*3]*m.data[1+0*3]*m.data[2+2*3] - m.data[0+1*3]*m.data[2+0*3]*m.data[1+2*3] -
		 m.data[0+2*3]*m.data[1+0*3]*m.data[2+1*3] + m.data[0+2*3]*m.data[2+0*3]*m.data[1+1*3];
}

bool Math::invert(Matrix3x3 &r,const Matrix3x3 m){
	real det=1.0/determinant(m);
	if(det==0.0){
		return false;
	}
	else{
		r.data[0+0*3]=-(det*((m.data[1+1*3]*m.data[2+2*3]) - (m.data[2+1*3]*m.data[1+2*3])));
		r.data[1+0*3]= (det*((m.data[1+0*3]*m.data[2+2*3]) - (m.data[2+0*3]*m.data[1+2*3])));
		r.data[2+0*3]=-(det*((m.data[1+0*3]*m.data[2+1*3]) - (m.data[2+0*3]*m.data[1+1*3])));

		r.data[0+1*3]= (det*((m.data[0+1*3]*m.data[2+2*3]) - (m.data[2+1*3]*m.data[0+2*3])));
		r.data[1+1*3]=-(det*((m.data[0+0*3]*m.data[2+2*3]) - (m.data[2+0*3]*m.data[0+2*3])));
		r.data[2+1*3]= (det*((m.data[0+0*3]*m.data[2+1*3]) - (m.data[2+0*3]*m.data[0+1*3])));

		r.data[0+2*3]=-(det*((m.data[0+1*3]*m.data[1+2*3]) - (m.data[1+1*3]*m.data[0+2*3])));
		r.data[1+2*3]= (det*((m.data[0+0*3]*m.data[1+2*3]) - (m.data[1+0*3]*m.data[0+2*3])));
		r.data[2+2*3]=-(det*((m.data[0+0*3]*m.data[1+1*3]) - (m.data[1+0*3]*m.data[0+1*3])));
		
		return true;
	}
}

void Math::transpose(Matrix4x4 &r,const Matrix4x4 &m){
	r.data[0+0*4]=m.data[0+0*4]; r.data[0+1*4]=m.data[1+0*4]; r.data[0+2*4]=m.data[2+0*4]; r.data[0+3*4]=m.data[3+0*4];
	r.data[1+0*4]=m.data[0+1*4]; r.data[1+1*4]=m.data[1+1*4]; r.data[1+2*4]=m.data[2+1*4]; r.data[1+3*4]=m.data[3+1*4];
	r.data[2+0*4]=m.data[0+2*4]; r.data[2+1*4]=m.data[1+2*4]; r.data[2+2*4]=m.data[2+2*4]; r.data[2+3*4]=m.data[3+2*4];
	r.data[3+0*4]=m.data[0+3*4]; r.data[3+1*4]=m.data[1+3*4]; r.data[3+2*4]=m.data[2+3*4]; r.data[3+3*4]=m.data[3+3*4];
}

real Math::determinant(const Matrix4x4 &m){
	real det1 = (m.data[1+2*4]*m.data[2+3*4]) - (m.data[2+2*4]*m.data[1+3*4]);
	real det2 = (m.data[1+1*4]*m.data[2+3*4]) - (m.data[2+1*4]*m.data[1+3*4]);
	real det3 = (m.data[1+1*4]*m.data[2+2*4]) - (m.data[2+1*4]*m.data[1+2*4]);
	real det4 = (m.data[1+0*4]*m.data[2+3*4]) - (m.data[2+0*4]*m.data[1+3*4]);
	real det5 = (m.data[1+0*4]*m.data[2+2*4]) - (m.data[2+0*4]*m.data[1+2*4]);
	real det6 = (m.data[1+0*4]*m.data[2+1*4]) - (m.data[2+0*4]*m.data[1+1*4]);

	return -(m.data[3+0*4]*((m.data[0+1*4]*det1) - (m.data[0+2*4]*det2) + (m.data[0+3*4]*det3))) +
			(m.data[3+1*4]*((m.data[0+0*4]*det1) - (m.data[0+2*4]*det4) + (m.data[0+3*4]*det5))) -
			(m.data[3+2*4]*((m.data[0+0*4]*det2) - (m.data[0+1*4]*det4) + (m.data[0+3*4]*det6))) +
			(m.data[3+3*4]*((m.data[0+0*4]*det3) - (m.data[0+1*4]*det5) + (m.data[0+2*4]*det6)));
}

bool Math::invert(Matrix4x4 &r,const Matrix4x4 &m){
	real det=determinant(m);

	if(abs(det)==0.0){
		return false;
	}
	else{
		det=1.0/det;
		r.data[0+0*4] = (det*((m.data[1+1*4]*((m.data[2+2*4]*m.data[3+3*4]) - (m.data[2+3*4]*m.data[3+2*4]))) + (m.data[1+2*4]*((m.data[2+3*4]*m.data[3+1*4]) - (m.data[2+1*4]*m.data[3+3*4]))) + (m.data[1+3*4]*((m.data[2+1*4]*m.data[3+2*4]) - (m.data[2+2*4]*m.data[3+1*4])))));
		r.data[0+1*4] = (det*((m.data[2+1*4]*((m.data[0+2*4]*m.data[3+3*4]) - (m.data[0+3*4]*m.data[3+2*4]))) + (m.data[2+2*4]*((m.data[0+3*4]*m.data[3+1*4]) - (m.data[0+1*4]*m.data[3+3*4]))) + (m.data[2+3*4]*((m.data[0+1*4]*m.data[3+2*4]) - (m.data[0+2*4]*m.data[3+1*4])))));
		r.data[0+2*4] = (det*((m.data[3+1*4]*((m.data[0+2*4]*m.data[1+3*4]) - (m.data[0+3*4]*m.data[1+2*4]))) + (m.data[3+2*4]*((m.data[0+3*4]*m.data[1+1*4]) - (m.data[0+1*4]*m.data[1+3*4]))) + (m.data[3+3*4]*((m.data[0+1*4]*m.data[1+2*4]) - (m.data[0+2*4]*m.data[1+1*4])))));
		r.data[0+3*4] = (det*((m.data[0+1*4]*((m.data[1+3*4]*m.data[2+2*4]) - (m.data[1+2*4]*m.data[2+3*4]))) + (m.data[0+2*4]*((m.data[1+1*4]*m.data[2+3*4]) - (m.data[1+3*4]*m.data[2+1*4]))) + (m.data[0+3*4]*((m.data[1+2*4]*m.data[2+1*4]) - (m.data[1+1*4]*m.data[2+2*4])))));

		r.data[1+0*4] = (det*((m.data[1+2*4]*((m.data[2+0*4]*m.data[3+3*4]) - (m.data[2+3*4]*m.data[3+0*4]))) + (m.data[1+3*4]*((m.data[2+2*4]*m.data[3+0*4]) - (m.data[2+0*4]*m.data[3+2*4]))) + (m.data[1+0*4]*((m.data[2+3*4]*m.data[3+2*4]) - (m.data[2+2*4]*m.data[3+3*4])))));
		r.data[1+1*4] = (det*((m.data[2+2*4]*((m.data[0+0*4]*m.data[3+3*4]) - (m.data[0+3*4]*m.data[3+0*4]))) + (m.data[2+3*4]*((m.data[0+2*4]*m.data[3+0*4]) - (m.data[0+0*4]*m.data[3+2*4]))) + (m.data[2+0*4]*((m.data[0+3*4]*m.data[3+2*4]) - (m.data[0+2*4]*m.data[3+3*4])))));
		r.data[1+2*4] = (det*((m.data[3+2*4]*((m.data[0+0*4]*m.data[1+3*4]) - (m.data[0+3*4]*m.data[1+0*4]))) + (m.data[3+3*4]*((m.data[0+2*4]*m.data[1+0*4]) - (m.data[0+0*4]*m.data[1+2*4]))) + (m.data[3+0*4]*((m.data[0+3*4]*m.data[1+2*4]) - (m.data[0+2*4]*m.data[1+3*4])))));
		r.data[1+3*4] = (det*((m.data[0+2*4]*((m.data[1+3*4]*m.data[2+0*4]) - (m.data[1+0*4]*m.data[2+3*4]))) + (m.data[0+3*4]*((m.data[1+0*4]*m.data[2+2*4]) - (m.data[1+2*4]*m.data[2+0*4]))) + (m.data[0+0*4]*((m.data[1+2*4]*m.data[2+3*4]) - (m.data[1+3*4]*m.data[2+2*4])))));

		r.data[2+0*4] = (det*((m.data[1+3*4]*((m.data[2+0*4]*m.data[3+1*4]) - (m.data[2+1*4]*m.data[3+0*4]))) + (m.data[1+0*4]*((m.data[2+1*4]*m.data[3+3*4]) - (m.data[2+3*4]*m.data[3+1*4]))) + (m.data[1+1*4]*((m.data[2+3*4]*m.data[3+0*4]) - (m.data[2+0*4]*m.data[3+3*4])))));
		r.data[2+1*4] = (det*((m.data[2+3*4]*((m.data[0+0*4]*m.data[3+1*4]) - (m.data[0+1*4]*m.data[3+0*4]))) + (m.data[2+0*4]*((m.data[0+1*4]*m.data[3+3*4]) - (m.data[0+3*4]*m.data[3+1*4]))) + (m.data[2+1*4]*((m.data[0+3*4]*m.data[3+0*4]) - (m.data[0+0*4]*m.data[3+3*4])))));
		r.data[2+2*4] = (det*((m.data[3+3*4]*((m.data[0+0*4]*m.data[1+1*4]) - (m.data[0+1*4]*m.data[1+0*4]))) + (m.data[3+0*4]*((m.data[0+1*4]*m.data[1+3*4]) - (m.data[0+3*4]*m.data[1+1*4]))) + (m.data[3+1*4]*((m.data[0+3*4]*m.data[1+0*4]) - (m.data[0+0*4]*m.data[1+3*4])))));
		r.data[2+3*4] = (det*((m.data[0+3*4]*((m.data[1+1*4]*m.data[2+0*4]) - (m.data[1+0*4]*m.data[2+1*4]))) + (m.data[0+0*4]*((m.data[1+3*4]*m.data[2+1*4]) - (m.data[1+1*4]*m.data[2+3*4]))) + (m.data[0+1*4]*((m.data[1+0*4]*m.data[2+3*4]) - (m.data[1+3*4]*m.data[2+0*4])))));

		r.data[3+0*4] = (det*((m.data[1+0*4]*((m.data[2+2*4]*m.data[3+1*4]) - (m.data[2+1*4]*m.data[3+2*4]))) + (m.data[1+1*4]*((m.data[2+0*4]*m.data[3+2*4]) - (m.data[2+2*4]*m.data[3+0*4]))) + (m.data[1+2*4]*((m.data[2+1*4]*m.data[3+0*4]) - (m.data[2+0*4]*m.data[3+1*4])))));
		r.data[3+1*4] = (det*((m.data[2+0*4]*((m.data[0+2*4]*m.data[3+1*4]) - (m.data[0+1*4]*m.data[3+2*4]))) + (m.data[2+1*4]*((m.data[0+0*4]*m.data[3+2*4]) - (m.data[0+2*4]*m.data[3+0*4]))) + (m.data[2+2*4]*((m.data[0+1*4]*m.data[3+0*4]) - (m.data[0+0*4]*m.data[3+1*4])))));
		r.data[3+2*4] = (det*((m.data[3+0*4]*((m.data[0+2*4]*m.data[1+1*4]) - (m.data[0+1*4]*m.data[1+2*4]))) + (m.data[3+1*4]*((m.data[0+0*4]*m.data[1+2*4]) - (m.data[0+2*4]*m.data[1+0*4]))) + (m.data[3+2*4]*((m.data[0+1*4]*m.data[1+0*4]) - (m.data[0+0*4]*m.data[1+1*4])))));
		r.data[3+3*4] = (det*((m.data[0+0*4]*((m.data[1+1*4]*m.data[2+2*4]) - (m.data[1+2*4]*m.data[2+1*4]))) + (m.data[0+1*4]*((m.data[1+2*4]*m.data[2+0*4]) - (m.data[1+0*4]*m.data[2+2*4]))) + (m.data[0+2*4]*((m.data[1+0*4]*m.data[2+1*4]) - (m.data[1+1*4]*m.data[2+0*4])))));

		return true;
	}
}

void Math::setMatrix4x4FromLookAt(Matrix4x4 &r,const Vector3 &eye,const Vector3 &point,const Vector3 &up,bool camera){
	Vector3 cacheDir;

	sub(cacheDir,point,eye);
	setMatrix4x4FromLookDir(r,eye,cacheDir,up,camera);
}

void Math::setMatrix4x4FromLookDir(Matrix4x4 &r,const Vector3 &eye,const Vector3 &dir,const Vector3 &up,bool camera){
	Vector3 cacheDir;
	Vector3 cacheUp;
	Vector3 cacheRight;

	normalizeCarefully(cacheDir,dir,0);
	normalizeCarefully(cacheUp,up,0);
	cross(cacheRight,cacheDir,cacheUp);
	normalizeCarefully(cacheRight,0);
	cross(cacheUp,cacheRight,cacheDir);

	if(camera){
		r.setAt(0,0,cacheRight.x);
		r.setAt(1,0,cacheRight.y);
		r.setAt(2,0,cacheRight.z);
		r.setAt(0,1,cacheUp.x);
		r.setAt(1,1,cacheUp.y);
		r.setAt(2,1,cacheUp.z);
		r.setAt(0,2,-cacheDir.x);
		r.setAt(1,2,-cacheDir.y);
		r.setAt(2,2,-cacheDir.z);
		r.setAt(0,3,eye.x);
		r.setAt(1,3,eye.y);
		r.setAt(2,3,eye.z);
	}
	else{
		Matrix3x3 rotate;
		rotate.setAt(0,0,cacheRight.x);
		rotate.setAt(0,1,cacheRight.y);
		rotate.setAt(0,2,cacheRight.z);
		rotate.setAt(1,0,cacheUp.x);
		rotate.setAt(1,1,cacheUp.y);
		rotate.setAt(1,2,cacheUp.z);
		rotate.setAt(2,0,-cacheDir.x);
		rotate.setAt(2,1,-cacheDir.y);
		rotate.setAt(2,2,-cacheDir.z);

		Vector3 reye;
		mul(reye,rotate,eye);

		r.setAt(0,0,cacheRight.x);
		r.setAt(0,1,cacheRight.y);
		r.setAt(0,2,cacheRight.z);
		r.setAt(1,0,cacheUp.x);
		r.setAt(1,1,cacheUp.y);
		r.setAt(1,2,cacheUp.z);
		r.setAt(2,0,-cacheDir.x);
		r.setAt(2,1,-cacheDir.y);
		r.setAt(2,2,-cacheDir.z);
		r.setAt(0,3,-reye.x);
		r.setAt(1,3,-reye.y);
		r.setAt(2,3,-reye.z);
	}
}

void Math::setMatrix4x4FromOrtho(Matrix4x4 &r,real leftDist,real rightDist,real bottomDist,real topDist,real nearDist,real farDist){
	r.set(
		2.0/(rightDist-leftDist),0,0,-(rightDist+leftDist)/(rightDist-leftDist),
		0,2.0/(topDist-bottomDist),0,-(topDist+bottomDist)/(topDist-bottomDist),
		0,0,-2.0/(farDist-nearDist),-(farDist+nearDist)/(farDist-nearDist),
		0,0,0,1.0
	);
}

void Math::setMatrix4x4FromFrustum(Matrix4x4 &r,real leftDist,real rightDist,real bottomDist,real topDist,real nearDist,real farDist){
	r.set(
		(2.0*nearDist)/(rightDist-leftDist),0,(rightDist+leftDist)/(rightDist-leftDist),0,
		0,(2.0*nearDist)/(topDist-bottomDist),(topDist+bottomDist)/(topDist-bottomDist),0,
		0,0,-(farDist+nearDist)/(farDist-nearDist),-(2*farDist*nearDist)/(farDist-nearDist),
		0,0,-1.0,0
	);
}

void Math::setMatrix4x4FromPerspectiveX(Matrix4x4 &r,real fovx,real aspect,real nearDist,real farDist){
	real h=tan(fovx/2.0);

	r.set(
		1.0/h,0,0,0,
		0,1.0/(h*aspect),0,0,
		0,0,(farDist+nearDist)/(nearDist-farDist),(farDist*nearDist*2.0)/(nearDist-farDist),
		0,0,-1.0,0
	);
}

void Math::setMatrix4x4FromPerspectiveY(Matrix4x4 &r,real fovy,real aspect,real nearDist,real farDist){
	real h=tan(fovy/2.0);

	r.set(
		1.0/(h*aspect),0,0,0,
		0,1.0/h,0,0,
		0,0,(farDist+nearDist)/(nearDist-farDist),(farDist*nearDist*2.0)/(nearDist-farDist),
		0,0,-1.0,0
	);
}

void Math::setMatrix4x4FromObliquePlane(Matrix4x4 &r,const Plane &plane,const Vector3 &dir){
	real nd=Math::dot(plane.normal,dir);

	r.set(
		dir.x*plane.normal.x - nd, dir.x*plane.normal.y, dir.x*plane.normal.z, -plane.distance*dir.x,
		dir.y*plane.normal.x, dir.y*plane.normal.y - nd, dir.y*plane.normal.z, -plane.distance*dir.y,
		dir.z*plane.normal.x, dir.z*plane.normal.y, dir.z*plane.normal.z - nd, -plane.distance*dir.z,
		0,0,0,-nd
	);
}

void Math::setMatrix4x4FromPerspectivePlane(Matrix4x4 &r,const Plane &plane,const Vector3 &eye){
	Vector3 cacheEp;

	cacheEp.set(eye.x-plane.normal.x*plane.distance,eye.y-plane.normal.y*plane.distance,eye.z-plane.normal.z*plane.distance);
	real nd=Math::dot(plane.normal,cacheEp);

	r.set(
		nd - eye.x*plane.normal.x,-eye.x*plane.normal.y,-eye.x*plane.normal.z, 0,
		-eye.y*plane.normal.x,nd - eye.y*plane.normal.y,-eye.y*plane.normal.z, 0,
		-eye.z*plane.normal.x,-eye.z*plane.normal.y,nd - eye.z*plane.normal.z, 0,
		-plane.normal.x,-plane.normal.y,-plane.normal.z,Math::dot(eye,plane.normal)
	);

	r.data[12] = -(r.data[0]*eye.x + r.data[4]*eye.y + r.data[8]*eye.z);
	r.data[13] = -(r.data[1]*eye.x + r.data[5]*eye.y + r.data[9]*eye.z);
	r.data[14]= -(r.data[2]*eye.x + r.data[6]*eye.y + r.data[10]*eye.z);
}

real Math::setAxisAngleFromQuaternion(Vector3 &axis,const Quaternion &q,real epsilon){
	real angle=2.0*acos(q.w);

	real sinHalfAngle=sin(angle*0.5);
	if(sinHalfAngle>epsilon){
		real invSinHalfAngle=1.0/sinHalfAngle;
		axis.x=q.x*invSinHalfAngle;
		axis.y=q.y*invSinHalfAngle;
		axis.z=q.z*invSinHalfAngle;

		real l=1.0/(sqrt((axis.x*axis.x+axis.y*axis.y+axis.z*axis.z)));
		axis.x*=l;
		axis.y*=l;
		axis.z*=l;
	}
	else{
		axis.x=1.0;
		axis.y=0.0;
		axis.z=0.0;
	}

	return angle;
}

// Algorithm from Ken Shoemake's article in 1987 SIGGRAPH course notes
// article "Quaternion Calculus and Fast Animation".
template<class Matrix>
void setQuaternionFromMatrix(Quaternion &r,const Matrix &m){
	real trace=m.at(0,0)+m.at(1,1)+m.at(2,2);
	real root;

	if(trace>0.0){
		// |w| > 1/2, may as well choose w > 1/2
		root=Math::sqrt(trace+1.0); // 2w
		r.w=0.5*root;
		root=0.5/root; // 1/(4w)
		r.x=(m.at(2,1)-m.at(1,2))*root;
		r.y=(m.at(0,2)-m.at(2,0))*root;
		r.z=(m.at(1,0)-m.at(0,1))*root;
	}
	else{
		// |w| <= 1/2
		static const int next[3]={1,2,0};
		int i=0;
		if(m.at(1,1)>m.at(0,0)){
			i=1;
		}
		if(m.at(2,2)>m.at(i,i)){
			i=2;
		}
		int j=next[i];
		int k=next[j];

		root=Math::sqrt(m.at(i,i)-m.at(j,j)-m.at(k,k)+1.0);
		r[i]=0.5*root;
		root=0.5/root;
		r.w=(m.at(k,j)-m.at(j,k))*root;
		r[j]=(m.at(j,i)+m.at(i,j))*root;
		r[k]=(m.at(k,i)+m.at(i,k))*root;
	}
}

void Math::setQuaternionFromMatrix3x3(Quaternion &r,const Matrix3x3 &m){ setQuaternionFromMatrix(r,m); }

void Math::setQuaternionFromMatrix4x4(Quaternion &r,const Matrix4x4 &m){ setQuaternionFromMatrix(r,m); }

void Math::lerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,real t){
	real cosom=dot(q1,q2);

	real scl1=(1.0-t);
	real scl2=0;
	if(cosom<0.0){
		scl2=-t;
	}
	else{
		scl2=t;
	}

	r.x=scl1*q1.x + scl2*q2.x;
	r.y=scl1*q1.y + scl2*q2.y;
	r.z=scl1*q1.z + scl2*q2.z;
	r.w=scl1*q1.w + scl2*q2.w;
}

// Algorithm from the GGT (http://ggt.sourceforge.net)
void Math::slerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,real t){
	real cosom=dot(q1,q2);

	real scl2sign=1.0;
	if(cosom<0.0){
		cosom=-cosom;
		scl2sign=-1.0;
	}

	real scl1=0;
	real scl2=0;
	if((1.0-cosom) > 0.0001){
		// Standard slerp
		real omega=acos(cosom);
		real sinom=sin(omega);
		scl1=sin((1.0-t) * omega)/sinom;
		scl2=sin(t*omega)/sinom;
	}
	else{
		// Very close, just lerp
		scl1=1.0-t;
		scl2=t;
	}
	scl2*=scl2sign;

	r.x=scl1*q1.x + scl2*q2.x;
	r.y=scl1*q1.y + scl2*q2.y;
	r.z=scl1*q1.z + scl2*q2.z;
	r.w=scl1*q1.w + scl2*q2.w;
}

void Math::project(Vector3 &result,const Segment &segment,const Vector3 &point,bool limitToSegment){
	const Vector3 &o=segment.origin;
	const Vector3 &d=segment.direction;
	
	if(d.x==0 && d.y==0 && d.z==0){
		result=o;
	}
	else{
		real u=(d.x*(point.x-o.x)+d.y*(point.y-o.y)+d.z*(point.z-o.z))/(d.x*d.x+d.y*d.y+d.z*d.z);
		if(limitToSegment){
			if(u<0) u=0;
			else if(u>1) u=1;
		}
		result.x=o.x+u*(d.x);
		result.y=o.y+u*(d.y);
		result.z=o.z+u*(d.z);
	}
}

void Math::project(Vector3 &point1,Vector3 &point2,const Segment &seg1, const Segment &seg2,real epsilon){
	// For a full discussion of this method see Dan Sunday's Geometry Algorithms web site: http://www.geometryalgorithms.com/
	real a=dot(seg1.direction,seg1.direction);
	real b=dot(seg1.direction,seg2.direction);
	real c=dot(seg2.direction,seg2.direction);

	// Make sure we don't have a very small segment somewhere, if so we treat it as a point
	if(a<=epsilon){
		point1=seg1.origin;
		project(point2,seg2,point1,true);
		return;
	}
	else if(c<epsilon){
		point2=seg2.origin;
		project(point1,seg1,point2,true);
		return;
	}

	// Use point1 as a temporary
	sub(point1,seg1.origin,seg2.origin);
	real d=dot(seg1.direction,point1);
	real e=dot(seg2.direction,point1);
	real denom=a*c-b*b;
	
	real u1=0;
	real u2=0;
	real u1N=0;
	real u2N=0;
	real u1D=denom;
	real u2D=denom;
	
	if(denom<0.000001){
		// Segments are practically parallel
		u1N=0;
		u1D=1;
		u2N=e;
		u2D=c;
	}
	else{
		// Find the closest point on 2 infinite lines
		u1N=(b*e-c*d);
		u2N=(a*e-b*d);
		
		// Check for closest point outside seg1 bounds
		if(u1N<0){
			u1N=0;
			u2N=e;
			u2D=c;
		}
		else if(u1N>u1D){
			u1N=u1D;
			u2N=e+b;
			u2D=c;
		}
	}
	
	// Check for closest point outside seg2 bounds
	if(u2N<0){
		u2N=0;
		// Recalculate u1N
		if(-d<0){
			u1N=0;
		}
		else if(-d>a){
			u1N=u1D;
		}
		else{
			u1N=-d;
			u1D=a;
		}
	}
	else if(u2N>u2D){
		u2N=u2D;
		// Recalculate u1N
		if((-d+b)<0){
			u1N=0;
		}
		else if((-d+b)>a){
			u1N=u1D;
		}
		else{
			u1N=(-d+b);
			u1D=a;
		}
	}
	
	u1=u1N/u1D;
	u2=u2N/u2D;
	
	mul(point1,seg1.direction,u1);
	add(point1,seg1.origin);
	mul(point2,seg2.direction,u2);
	add(point2,seg2.origin);
}

bool Math::unprojectSegment(Segment &result,const Matrix4x4 &projViewMatrix,int x,int y,unsigned int viewportX,unsigned int viewportY,unsigned int viewportWidth,unsigned int viewportHeight,Matrix4x4 &scratchMatrix){
	if(viewportWidth==0 || viewportHeight==0){
		return false;
	}

	invert(scratchMatrix,projViewMatrix);

	real fx=(real)(x-(int)viewportX)*2.0/(real)viewportWidth-1.0;
	real fy=(real)((int)viewportHeight-(y-(int)viewportY))*2.0/(real)viewportHeight-1.0;

	result.origin.set(fx,fy,-1.0f);
	mulPoint3Full(result.origin,scratchMatrix);

	result.direction.set(fx,fy,1.0f);
	mulPoint3Full(result.direction,scratchMatrix);

	sub(result.direction,result.origin);

	return true;
}

bool Math::getIntersectionOfThreePlanes(Vector3 &result,const Plane &p1,const Plane &p2,const Plane &p3,real epsilon){
	Vector3 p2xp3;
	cross(p2xp3,p2.normal,p3.normal);
	real den=dot(p1.normal,p2xp3);
	if(den<epsilon && den>-epsilon){
		return false;
	}
	else{
		Vector3 p3xp1,p1xp2;
		cross(p3xp1,p3.normal,p1.normal);
		cross(p1xp2,p1.normal,p2.normal);
		mul(p1xp2,p3.distance);
		mul(p2xp3,p1.distance);
		mul(p3xp1,p2.distance);
		add(result,p1xp2,p2xp3);
		add(result,p3xp1);
		div(result,den);
		return true;
	}
}

void Math::getLineOfIntersection(Segment &result,const Plane &plane1,const Plane &plane2){
	real determinant=lengthSquared(plane1.normal)*lengthSquared(plane2.normal) - (dot(plane1.normal,plane2.normal)*dot(plane1.normal,plane2.normal));

	real c1=(plane1.distance*lengthSquared(plane2.normal) - plane2.distance*dot(plane1.normal,plane2.normal))/determinant;
	real c2=(plane2.distance*lengthSquared(plane1.normal) - plane1.distance*dot(plane1.normal,plane2.normal))/determinant;

	cross(result.direction,plane1.normal,plane2.normal);
	Vector3 n1,n2;
	mul(n1,plane1.normal,c1);
	mul(n2,plane2.normal,c2);
	add(result.origin,n1,n2);
}

void Math::findBoundingBox(AABox &r,const Sphere &sphere){
	scalar radius=sphere.radius;
	r.mins.x=-radius;
	r.mins.y=-radius;
	r.mins.z=-radius;
	r.maxs.x=radius;
	r.maxs.y=radius;
	r.maxs.z=radius;
	Math::add(r,sphere.origin);
}

void Math::findBoundingBox(AABox &r,const Capsule &capsule){
	scalar radius=capsule.radius;
	const Vector3 &direction=capsule.direction;

	if(direction.x<0){
		r.mins.x=direction.x-radius;
		r.maxs.x=radius;
	}
	else{
		r.mins.x=-radius;
		r.maxs.x=direction.x+radius;
	}
	if(direction.y<0){
		r.mins.y=direction.y-radius;
		r.maxs.y=radius;
	}
	else{
		r.mins.y=-radius;
		r.maxs.y=direction.y+radius;
	}
	if(direction.z<0){
		r.mins.z=direction.z-radius;
		r.maxs.z=radius;
	}
	else{
		r.mins.z=-radius;
		r.maxs.z=direction.z+radius;
	}

	Math::add(r,capsule.origin);
}

void Math::findBoundingBox(AABox &r,const Vector3 *vertexes,int amount){
	int i;

	real f=0;

	for(i=0;i<amount;++i){
		const Vector3 &v=vertexes[i];

		if(f<abs(v.x))
			f=abs(v.x);
		if(f<abs(v.y))
			f=abs(v.y);
		if(f<abs(v.z))
			f=abs(v.z);
	}

	r.setMins(f,f,f);
	r.setMaxs(-f,-f,-f);
}

void Math::findFitBox(AABox &r,const Vector3 *vertexes,int amount){
	if(amount>0){
		r.maxs.set(vertexes[0]);
		r.mins.set(vertexes[0]);
	}
	
	int i;
	for(i=1;i<amount;++i){
		const Vector3 &v=vertexes[i];

		if(r.maxs.x<v.x)
			r.maxs.x=v.x;
		if(r.maxs.y<v.y)
			r.maxs.y=v.y;
		if(r.maxs.z<v.z)
			r.maxs.z=v.z;
		
		if(r.mins.x>v.x)
			r.mins.x=v.x;
		if(r.mins.y>v.y)
			r.mins.y=v.y;
		if(r.mins.z>v.z)
			r.mins.z=v.z;
	}
}

void Math::findBoundingSphere(Sphere &r,const AABox &box){
	const Vector3 &mins=box.mins;
	const Vector3 &maxs=box.maxs;

	r.origin.x=0.5*(mins.x+maxs.x);
	r.origin.y=0.5*(mins.y+maxs.y);
	r.origin.z=0.5*(mins.z+maxs.z);

	r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y)+(maxs.z-r.origin.z)*(maxs.z-r.origin.z)));
}

void Math::findFitSphere(Sphere &r,const AABox &box){
	const Vector3 &mins=box.mins;
	const Vector3 &maxs=box.maxs;

	r.origin.x=0.5*(mins.x+maxs.x);
	r.origin.y=0.5*(mins.y+maxs.y);
	r.origin.z=0.5*(mins.z+maxs.z);

	r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y)+(maxs.z-r.origin.z)*(maxs.z-r.origin.z))/sqrt(3));
}

void Math::findBoundingCapsule(Capsule &r,const AABox &box){
	const Vector3 &mins=box.mins;
	const Vector3 &maxs=box.mins;

	r.origin.x=0.5*(mins.x+maxs.x);
	r.origin.y=0.5*(mins.y+maxs.y);
	r.origin.z=mins.z;

	r.direction.z=(maxs.z-mins.z);

	r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y)));
}

void Math::findFitCapsule(Capsule &r,const AABox &box){
	const Vector3 &mins=box.mins;
	const Vector3 &maxs=box.mins;

	r.origin.x=0.5*(mins.x+maxs.x);
	r.origin.y=0.5*(mins.y+maxs.y);

	r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y))/SQRT_2);

	r.origin.z=mins.z+r.radius;

	r.direction.z=(maxs.z-mins.z-r.getRadius());

	if(r.direction.z<0){
		r.direction.z=0;
	}
}

bool Math::testInside(const Plane &plane,const Vector3 &point){
	return dot(point,plane.normal)<=plane.distance;
}

bool Math::testInside(const Sphere &sphere,const Vector3 &point){
	return lengthSquared(point,sphere.origin)<=square(sphere.radius);
}

bool Math::testInside(const AABox &box,const Vector3 &point){
	return	point.x>=box.mins.x && point.y>=box.mins.y && point.z>=box.mins.z &&
			point.x<=box.maxs.x && point.y<=box.maxs.y && point.z<=box.maxs.z;
}

bool Math::testIntersection(const AABox &box1,const AABox &box2){
	return	!(box1.mins.x>box2.maxs.x || box1.mins.y>box2.maxs.y || box1.mins.z>box2.maxs.z ||
			  box2.mins.x>box1.maxs.x || box2.mins.y>box1.maxs.y || box2.mins.z>box1.maxs.z);
}

bool Math::testIntersection(const Sphere &sphere1,const Sphere &sphere2){
	return lengthSquared(sphere1.origin,sphere2.origin)<=square(sphere1.radius+sphere2.radius);
}

bool Math::testIntersection(const Sphere &sphere,const AABox &box){
	real s=0;
	real d=0;

	if(sphere.origin.x<box.mins.x){
		s=sphere.origin.x-box.mins.x;
		d+=s*s;
	}
	else if(sphere.origin.x>box.maxs.x){
		s=sphere.origin.x-box.maxs.x;
		d+=s*s;
	}

	if(sphere.origin.y<box.mins.y){
		s=sphere.origin.y-box.mins.y;
		d+=s*s;
	}
	else if(sphere.origin.y>box.maxs.y){
		s=sphere.origin.y-box.maxs.y;
		d+=s*s;
	}

	if(sphere.origin.z<box.mins.z){
		s=sphere.origin.z-box.mins.z;
		d+=s*s;
	}
	else if(sphere.origin.z>box.maxs.z){
		s=sphere.origin.z-box.maxs.z;
		d+=s*s;
	}

	return d<=sphere.radius*sphere.radius;
}

real Math::findIntersection(const Segment &segment,const Plane &plane,Vector3 &point,Vector3 &normal){
	real d=dot(plane.normal,segment.direction);
	if(d!=0){
		real t=(plane.distance-dot(plane.normal,segment.origin))/d;
		mul(point,segment.direction,t);
		add(point,segment.origin);
		normal.set(plane.normal);
		if(t<0 || t>1.0){
			return 1.0;
		}
		else{
			return t;
		}
	}
	else{
		return 1.0;
	}
}

real Math::findIntersection(const Segment &segment,const Sphere &sphere,Vector3 &point,Vector3 &normal){
	const Vector3 &segOrigin=segment.origin;
	const Vector3 &segDirection=segment.direction;
	const Vector3 &sphOrigin=sphere.origin;
	real sphRadius=sphere.radius;

	// Quadratic Q(t) = a*t^2 + 2*b*t + c
	Vector3 diff;
	sub(diff,segOrigin,sphOrigin);
	real a=lengthSquared(segDirection);
	if(a<=0.0){
		return 1.0;
	}
	real b=dot(diff,segDirection);
	real c=lengthSquared(diff)-square(sphRadius);
	real time1=1.0;
	real time2=1.0;

	// If no real roots, then no intersection
	real discr=b*b - a*c;
	if(discr<0.0){
		time1=1.0;
	}
	else if(discr>0.0){
		real root=sqrt(discr);
		real invA=1.0/a;
		time1=((-b)-root)*invA;
		time2=((-b)+root)*invA;

		if(time1>1.0 || time2<0.0){
			time1=1.0;
		}
		else if(time1>=0.0){
			mul(point,segDirection,time1);
			add(point,segOrigin);
		}
		else{
			mul(point,segDirection,time2);
			add(point,segOrigin);
		}
	}
	else{
		time1=-b/a;
		if(0.0<=time1 && time1<=1.0){
			mul(point,segDirection,time1);
			add(point,segOrigin);
		}
		else{
			time1=1.0;
		}
	}

	if(time1!=1.0){
		sub(normal,point,sphOrigin);
		normalizeCarefully(normal,0);
	}
	return time1;
}

real Math::findIntersection(const Segment &segment,const AABox &box,Vector3 &point,Vector3 &normal){
	const Vector3 &segOrigin=segment.origin;
	const Vector3 &segDirection=segment.direction;
	const Vector3 &boxMins=box.mins;
	const Vector3 &boxMaxs=box.maxs;
	
	bool inside=true;
	char quadX,quadY,quadZ;
	int whichPlane;
	real maxTX=0,maxTY=0,maxTZ=0;
	real candPlaneX=0,candPlaneY=0,candPlaneZ=0;
	real candNormX=-1.0;
	real candNormY=-1.0;
	real candNormZ=-1.0;
	real time=1.0;

	// The below tests were originally < or >, but are now <= or >=
	// Without this change finds that start on the edge of a box count as inside which conflicts with how testInside works

	// X
	if(segOrigin.x<=boxMins.x){
		quadX=0; // Left
		candPlaneX=boxMins.x;
		inside=false;
	}
	else if(segOrigin.x>=boxMaxs.x){
		quadX=1; // Right
		candPlaneX=boxMaxs.x;
		candNormX=1.0;
		inside=false;
	}
	else{
		quadX=2; // Middle
	}
	// Y
	if(segOrigin.y<=boxMins.y){
		quadY=0; // Left
		candPlaneY=boxMins.y;
		inside=false;
	}
	else if(segOrigin.y>=boxMaxs.y){
		quadY=1; // Right
		candPlaneY=boxMaxs.y;
		candNormY=1.0;
		inside=false;
	}
	else{
		quadY=2; // Middle
	}
	// Z
	if(segOrigin.z<=boxMins.z){
		quadZ=0; // Left
		candPlaneZ=boxMins.z;
		inside=false;
	}
	else if(segOrigin.z>=boxMaxs.z){
		quadZ=1; // Right
		candPlaneZ=boxMaxs.z;
		candNormZ=1.0;
		inside=false;
	}
	else{
		quadZ=2; // Middle
	}

	// Inside
	if(inside){
		return 0.0;
	}

	// Calculate t distances to candidate planes
	// X
	if(quadX!=2 /*Middle*/ && segDirection.x!=0){
		maxTX=(candPlaneX-segOrigin.x)/segDirection.x;
	}
	else{
		maxTX=-1.0;
	}
	// Y
	if(quadY!=2 /*Middle*/ && segDirection.y!=0){
		maxTY=(candPlaneY-segOrigin.y)/segDirection.y;
	}
	else{
		maxTY=-1.0;
	}
	// Z
	if(quadZ!=2 /*Middle*/ && segDirection.z!=0){
		maxTZ=(candPlaneZ-segOrigin.z)/segDirection.z;
	}
	else{
		maxTZ=-1.0;
	}

	// Find largest of maxT's
	// X
	if(maxTX>maxTY && maxTX>maxTZ){
		whichPlane=0;
		time=maxTX;
		normal.x=candNormX;normal.y=0;normal.z=0;
	}
	// Y
	else if(maxTY>maxTZ){
		whichPlane=1;
		time=maxTY;
		normal.x=0;normal.y=candNormY;normal.z=0;
	}
	// Z
	else{
		whichPlane=2;
		time=maxTZ;
		normal.x=0;normal.y=0;normal.z=candNormZ;
	}

	// Check final candidate actually inside box and calculate final point
	if(time<0.0 || time>1.0){
		return 1.0;
	}
	// X
	if(whichPlane!=0){
		point.x=segOrigin.x+time*segDirection.x;
		if(point.x<boxMins.x || point.x>boxMaxs.x){
			return 1.0;
		}
	}
	else{
		point.x=candPlaneX;
	}
	// Y
	if(whichPlane!=1){
		point.y=segOrigin.y+time*segDirection.y;
		if(point.y<boxMins.y || point.y>boxMaxs.y){
			return 1.0;
		}
	}
	else{
		point.y=candPlaneY;
	}
	// Z
	if(whichPlane!=2){
		point.z=segOrigin.z+time*segDirection.z;
		if(point.z<boxMins.z || point.z>boxMaxs.z){
			return 1.0;
		}
	}
	else{
		point.z=candPlaneZ;
	}
	
	return time;
}

}
}
}
