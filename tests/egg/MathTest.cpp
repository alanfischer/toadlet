#if 1

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/mathfixed/Math.h>
#include "../quicktest.h"

using namespace toadlet::egg;
using namespace toadlet::egg::math;

QT_TEST(MathTest){
	EulerAngle euler1,euler2;
	Quaternion quaternion1,quaternion2;
	Matrix3x3 matrix1,matrix2;
	real epsilon=0.0001;
	bool result1;

	Math::setQuaternionFromEulerAngleXYZ(quaternion1,euler1.set(0.25,0.5,0.75));
	result1=Math::setEulerAngleXYZFromQuaternion(euler2,quaternion1,epsilon);
	QT_CHECK(result1 && Math::abs(euler1.x-euler2.x)<epsilon && Math::abs(euler1.y-euler2.y)<epsilon && Math::abs(euler1.z-euler2.z)<epsilon);

	Math::setMatrix3x3FromEulerAngleXYZ(matrix1,euler1.set(0.25,0.5,0.75));
	result1=Math::setEulerAngleXYZFromMatrix3x3(euler2,matrix1,epsilon);
	QT_CHECK(result1 && Math::abs(euler1.x-euler2.x)<epsilon && Math::abs(euler1.y-euler2.y)<epsilon && Math::abs(euler1.z-euler2.z)<epsilon);

	Math::setQuaternionFromEulerAngleXYZ(quaternion1,euler1.set(0.25,0.5,0.75));
	Math::setMatrix3x3FromQuaternion(matrix1,quaternion1);
	result1=Math::setEulerAngleXYZFromMatrix3x3(euler2,matrix1,epsilon);
	QT_CHECK(result1 && Math::abs(euler1.x-euler2.x)<epsilon && Math::abs(euler1.y-euler2.y)<epsilon && Math::abs(euler1.z-euler2.z)<epsilon);

	Math::setMatrix3x3FromEulerAngleXYZ(matrix1,euler1.set(0.25,0.5,0.75));
	Math::setQuaternionFromMatrix3x3(quaternion1,matrix1);
	result1=Math::setEulerAngleXYZFromQuaternion(euler2,quaternion1,epsilon);
	QT_CHECK(result1 && Math::abs(euler1.x-euler2.x)<epsilon && Math::abs(euler1.y-euler2.y)<epsilon && Math::abs(euler1.z-euler2.z)<epsilon);
}

#endif