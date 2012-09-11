#ifndef MSCONVERSION_H
#define MSCONVERSION_H

#include "msLib.h"
#include <toadlet/egg/math/Math.h>

using namespace toadlet::egg::math;

Matrix3x3 milkshapeToToadlet;
Matrix3x3 toadletToMilkshape;

static class Initializer{
public: Initializer(){
		Math::setMatrix3x3FromX(milkshapeToToadlet,Math::degToRad(90));
		Math::setMatrix3x3FromX(toadletToMilkshape,Math::degToRad(-90));
	}
} init;

inline void convertVector3ToMSVec3(const Vector3 &tvec,msVec3 msvec,bool rotate){
	Vector3 temp(tvec);
	if(rotate){ Math::mul(temp,toadletToMilkshape); }
	msvec[0]=temp.x;
	msvec[1]=temp.y;
	msvec[2]=temp.z;
}

inline void convertMSVec3ToVector3(const msVec3 &msvec,Vector3 &tvec,bool rotate){
	tvec.x=msvec[0];
	tvec.y=msvec[1];
	tvec.z=msvec[2];
	if(rotate){ Math::mul(tvec,milkshapeToToadlet); }
}

/// @todo: Till we finalize the EulerAngle stuff in toadlet, and add in conversion for all possible rotation types,
//  we're just going to have some custom MS math routines

inline void setMatrix3x3FromEulerAngleMS(Matrix3x3 &r,const EulerAngle &euler){
	Matrix3x3 xmat,ymat,zmat;
	Math::setMatrixFromX(xmat,euler.x);
	Math::setMatrixFromY(ymat,euler.y);
	Math::setMatrixFromZ(zmat,euler.z);
	Math::postMul(ymat,zmat);
	Math::mul(r,xmat,ymat);
}

inline bool setEulerAngleMSFromMatrix3x3(EulerAngle &r,const Matrix3x3 &m){
	r.y=Math::asin(m.at(0,2));
	if(r.y<Math::HALF_PI){
		if(r.y>-Math::HALF_PI){
			r.x=Math::atan2(-m.at(1,2),m.at(2,2));
			r.z=Math::atan2(-m.at(0,1),m.at(0,0));
			return true;
		}
		else{
			real my=Math::atan2(m.at(1,0),m.at(1,1));
			r.z=0.0;
			r.x=r.z-my;
			return false;
		}
	}
	else{
		real py=Math::atan2(m.at(1,0),m.at(1,1));
		r.z=0.0;
		r.x=py-r.z;
		return false;
	}
}

inline void convertQuaternionToMSVec3(const Quaternion &quat,msVec3 msvec,bool rotate){
	if(Math::length(quat,Quaternion(0.5f,-0.5f,-0.5f,-0.5f))<0.0001f){
		msvec[0]=-Math::HALF_PI;
		msvec[1]=Math::HALF_PI;
		msvec[2]=0;
		return;
	}
	else if(Math::length(quat,Quaternion(-0.5f,-0.5f,0.5f,-0.5f))<0.0001f){
		msvec[0]=Math::HALF_PI;
		msvec[1]=Math::HALF_PI;
		msvec[2]=0;
		return;
	}
	else if(Math::length(quat,Quaternion(0.0f,0.0f,-0.707107f,-0.707107f))<0.0001f){
		msvec[0]=-Math::HALF_PI;
		msvec[1]=0;
		msvec[2]=0;
		return;
	}

	Quaternion temp(quat);
	temp.w*=-1.0f;
	Math::normalize(temp);

	Matrix3x3 matrix;
	Math::setMatrix3x3FromQuaternion(matrix,temp);
	if(rotate){
		// This multiply uses milkshapeToToadlet, seems odd but it works
		matrix=matrix*milkshapeToToadlet;
	}
	EulerAngle angle;
	setEulerAngleMSFromMatrix3x3(angle,matrix);

	msvec[0]=-angle.x;
	msvec[1]=-angle.y;
	msvec[2]=-angle.z;
}

inline void convertMSVec3ToQuaternion(const msVec3 &msvec,Quaternion &quat,bool rotate){
	EulerAngle angle(EulerAngle::EulerOrder_XYZ,-msvec[0],-msvec[1],-msvec[2]);

	Matrix3x3 matrix;
	setMatrix3x3FromEulerAngleMS(matrix,angle);
	if(rotate){
		// This multiply uses toadletToMilkshape, seems odd but it works
		matrix=matrix*toadletToMilkshape;
	}
	Math::setQuaternionFromMatrix3x3(quat,matrix);

	quat.w*=-1.0f;
	Math::normalize(quat);
}

#endif
