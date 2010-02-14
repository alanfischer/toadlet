#if 1

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/mathfixed/Math.h>
#include "../quicktest.h"

using namespace toadlet::egg;
using namespace toadlet::egg::math;

QT_TEST(MathTest){
	EulerAngle euler(Math::HALF_PI,-Math::PI,Math::QUARTER_PI);
	Quaternion quaternion;
	Math::setQuaternionFromEulerAngleXYZ(quaternion,euler);
	Matrix3x3 matrix;
	Math::setMatrix3x3FromQuaternion(matrix,quaternion);
	EulerAngle euler2;
	Math::setEulerAngleXYZFromMatrix3x3(euler2,matrix);
	QT_CHECK(Math::abs(euler.x-euler2.x)<0.001 && Math::abs(euler.y-euler2.y)<0.001 && Math::abs(euler.z-euler2.z)<0.001);
}

#endif