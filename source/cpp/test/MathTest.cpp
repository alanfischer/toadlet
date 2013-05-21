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

#include "MathTest.h"

real epsilon=0.0001;

bool MathTest::testEulerAngle(const EulerAngle &angle1,const EulerAngle &angle2){
	return
		(Math::abs(angle1.x-angle2.x)<epsilon) &&
		(Math::abs(angle1.y-angle2.y)<epsilon) &&
		(Math::abs(angle1.z-angle2.z)<epsilon);
}

bool MathTest::testEulerAngleConversion(){
	EulerAngle angle1,angle2;
	Quaternion quat;
	Matrix3x3 matrix;
	bool result=true;

	Log::alert(" Testing EulerAngle -> Quaternion -> EulerAngle");

	angle1.set(EulerAngle::EulerOrder_XYZ,1.0,1.2,1.4);
	angle2.order=EulerAngle::EulerOrder_XYZ;

	Math::setQuaternionFromEulerAngle(quat,angle1);
	if(Math::setEulerAngleFromQuaternion(angle2,quat,epsilon)==false){
		Log::alert("Singularity");
	}

	if(testEulerAngle(angle1,angle2)==false){
		Log::alert(MathFormatting::formatEulerAngle(angle1)+"!="+MathFormatting::formatEulerAngle(angle2));
		result=false;
	}

	angle1.set(EulerAngle::EulerOrder_ZYX,1.0,1.2,1.4);
	angle2.order=EulerAngle::EulerOrder_ZYX;

	Math::setQuaternionFromEulerAngle(quat,angle1);
	if(Math::setEulerAngleFromQuaternion(angle2,quat,epsilon)==false){
		Log::alert("Singularity");
	}

	if(testEulerAngle(angle1,angle2)==false){
		Log::alert(MathFormatting::formatEulerAngle(angle1)+"!="+MathFormatting::formatEulerAngle(angle2));
		result=false;
	}

	angle1.set(EulerAngle::EulerOrder_ZXY,1.0,1.2,1.4);
	angle2.order=EulerAngle::EulerOrder_ZXY;

	Math::setQuaternionFromEulerAngle(quat,angle1);
	if(Math::setEulerAngleFromQuaternion(angle2,quat,epsilon)==false){
		Log::alert("Singularity");
	}

	if(testEulerAngle(angle1,angle2)==false){
		Log::alert(MathFormatting::formatEulerAngle(angle1)+"!="+MathFormatting::formatEulerAngle(angle2));
		result=false;
	}

	Log::alert(" Testing EulerAngle -> Matrix -> EulerAngle");

	angle1.set(EulerAngle::EulerOrder_XYZ,1.0,1.2,1.4);
	angle2.order=EulerAngle::EulerOrder_XYZ;

	Math::setMatrix3x3FromEulerAngle(matrix,angle1,epsilon);
	if(Math::setEulerAngleFromMatrix3x3(angle2,matrix,epsilon)==false){
		Log::alert("Singularity");
	}

	if(testEulerAngle(angle1,angle2)==false){
		Log::alert(MathFormatting::formatEulerAngle(angle1)+"!="+MathFormatting::formatEulerAngle(angle2));
		result=false;
	}

	angle1.set(EulerAngle::EulerOrder_ZYX,1.0,1.2,1.4);
	angle2.order=EulerAngle::EulerOrder_ZYX;

	Math::setMatrix3x3FromEulerAngle(matrix,angle1,epsilon);
	if(Math::setEulerAngleFromMatrix3x3(angle2,matrix,epsilon)==false){
		Log::alert("Singularity");
	}

	if(testEulerAngle(angle1,angle2)==false){
		Log::alert(MathFormatting::formatEulerAngle(angle1)+"!="+MathFormatting::formatEulerAngle(angle2));
		result=false;
	}

	angle1.set(EulerAngle::EulerOrder_ZXY,1.0,1.2,1.4);
	angle2.order=EulerAngle::EulerOrder_ZXY;

	Math::setMatrix3x3FromEulerAngle(matrix,angle1,epsilon);
	if(Math::setEulerAngleFromMatrix3x3(angle2,matrix,epsilon)==false){
		Log::alert("Singularity");
	}

	if(testEulerAngle(angle1,angle2)==false){
		Log::alert(MathFormatting::formatEulerAngle(angle1)+"!="+MathFormatting::formatEulerAngle(angle2));
		result=false;
	}

	return result;
}

bool MathTest::run(){
	bool result=false;

	Log::alert("Testing EulerAngles");
	result=testEulerAngleConversion();
	Log::alert(result?"Succeeded":"Failed");

	return result;
}
