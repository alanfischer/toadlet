#if 1

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/mathfixed/Math.h>
#include "../quicktest.h"

using namespace toadlet::egg;

QT_TEST(MathTest){
	math::EulerAngle euler1,euler2;
	math::Quaternion quaternion1,quaternion2;
	math::Matrix3x3 matrix1,matrix2;
	math::real epsilon=0.0001;
	bool result1;

	euler1.set(math::Math::fromMilli(250),math::Math::fromMilli(500),math::Math::fromMilli(750));

	math::Math::setQuaternionFromEulerAngleXYZ(quaternion1,euler1);
	result1=math::Math::setEulerAngleXYZFromQuaternion(euler2,quaternion1,epsilon);
	QT_CHECK(result1 && math::Math::abs(euler1.x-euler2.x)<epsilon && math::Math::abs(euler1.y-euler2.y)<epsilon && math::Math::abs(euler1.z-euler2.z)<epsilon);

	math::Math::setMatrix3x3FromEulerAngleXYZ(matrix1,euler1);
	result1=math::Math::setEulerAngleXYZFromMatrix3x3(euler2,matrix1,epsilon);
	QT_CHECK(result1 && math::Math::abs(euler1.x-euler2.x)<epsilon && math::Math::abs(euler1.y-euler2.y)<epsilon && math::Math::abs(euler1.z-euler2.z)<epsilon);

	math::Math::setQuaternionFromEulerAngleXYZ(quaternion1,euler1);
	math::Math::setMatrix3x3FromQuaternion(matrix1,quaternion1);
	result1=math::Math::setEulerAngleXYZFromMatrix3x3(euler2,matrix1,epsilon);
	QT_CHECK(result1 && math::Math::abs(euler1.x-euler2.x)<epsilon && math::Math::abs(euler1.y-euler2.y)<epsilon && math::Math::abs(euler1.z-euler2.z)<epsilon);

	math::Math::setMatrix3x3FromEulerAngleXYZ(matrix1,euler1);
	math::Math::setQuaternionFromMatrix3x3(quaternion1,matrix1);
	result1=math::Math::setEulerAngleXYZFromQuaternion(euler2,quaternion1,epsilon);
	QT_CHECK(result1 && math::Math::abs(euler1.x-euler2.x)<epsilon && math::Math::abs(euler1.y-euler2.y)<epsilon && math::Math::abs(euler1.z-euler2.z)<epsilon);
}

QT_TEST(MathFixedTest){
	mathfixed::EulerAngle euler1,euler2;
	mathfixed::Quaternion quaternion1,quaternion2;
	mathfixed::Matrix3x3 matrix1,matrix2;
	mathfixed::fixed epsilon=mathfixed::Math::fromMilli(75);
	bool result1;

	euler1.set(mathfixed::Math::fromMilli(250),mathfixed::Math::fromMilli(500),mathfixed::Math::fromMilli(750));
	mathfixed::Math::setQuaternionFromEulerAngleXYZ(quaternion1,euler1);
	result1=mathfixed::Math::setEulerAngleXYZFromQuaternion(euler2,quaternion1,epsilon);
	QT_CHECK(result1 && mathfixed::Math::abs(euler1.x-euler2.x)<epsilon && mathfixed::Math::abs(euler1.y-euler2.y)<epsilon && mathfixed::Math::abs(euler1.z-euler2.z)<epsilon);

	mathfixed::Math::setMatrix3x3FromEulerAngleXYZ(matrix1,euler1);
	result1=mathfixed::Math::setEulerAngleXYZFromMatrix3x3(euler2,matrix1,epsilon);
	QT_CHECK(result1 && mathfixed::Math::abs(euler1.x-euler2.x)<epsilon && mathfixed::Math::abs(euler1.y-euler2.y)<epsilon && mathfixed::Math::abs(euler1.z-euler2.z)<epsilon);

	mathfixed::Math::setQuaternionFromEulerAngleXYZ(quaternion1,euler1);
	mathfixed::Math::setMatrix3x3FromQuaternion(matrix1,quaternion1);
	result1=mathfixed::Math::setEulerAngleXYZFromMatrix3x3(euler2,matrix1,epsilon);
	QT_CHECK(result1 && mathfixed::Math::abs(euler1.x-euler2.x)<epsilon && mathfixed::Math::abs(euler1.y-euler2.y)<epsilon && mathfixed::Math::abs(euler1.z-euler2.z)<epsilon);

	mathfixed::Math::setMatrix3x3FromEulerAngleXYZ(matrix1,euler1);
	mathfixed::Math::setQuaternionFromMatrix3x3(quaternion1,matrix1);
	result1=mathfixed::Math::setEulerAngleXYZFromQuaternion(euler2,quaternion1,epsilon);
	QT_CHECK(result1 && mathfixed::Math::abs(euler1.x-euler2.x)<epsilon && mathfixed::Math::abs(euler1.y-euler2.y)<epsilon && mathfixed::Math::abs(euler1.z-euler2.z)<epsilon);
}

#endif