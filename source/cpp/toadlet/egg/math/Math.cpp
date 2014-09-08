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
#include <toadlet/egg/Assert.h>
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/System.h>
#if defined(TOADLET_HAS_SSE)
	#include <xmmintrin.h> // SSE
#endif

namespace toadlet{
namespace egg{
namespace math{

void Math::add(Matrix3x3 &r,const Matrix3x3 &m1,const Matrix3x3 &m2){
	r.data[0+0*3]=m1.data[0+0*3] + m2.data[0+0*3];
	r.data[0+1*3]=m1.data[0+1*3] + m2.data[0+1*3];
	r.data[0+2*3]=m1.data[0+2*3] + m2.data[0+2*3];

	r.data[1+0*3]=m1.data[1+0*3] + m2.data[1+0*3];
	r.data[1+1*3]=m1.data[1+1*3] + m2.data[1+1*3];
	r.data[1+2*3]=m1.data[1+2*3] + m2.data[1+2*3];

	r.data[2+0*3]=m1.data[2+0*3] + m2.data[2+0*3];
	r.data[2+1*3]=m1.data[2+1*3] + m2.data[2+1*3];
	r.data[2+2*3]=m1.data[2+2*3] + m2.data[2+2*3];
}

void Math::add(Matrix3x3 &m1,const Matrix3x3 &m2){
	m1.data[0+0*3]+=m2.data[0+0*3];
	m1.data[0+1*3]+=m2.data[0+1*3];
	m1.data[0+2*3]+=m2.data[0+2*3];

	m1.data[1+0*3]+=m2.data[1+0*3];
	m1.data[1+1*3]+=m2.data[1+1*3];
	m1.data[1+2*3]+=m2.data[1+2*3];

	m1.data[2+0*3]+=m2.data[2+0*3];
	m1.data[2+1*3]+=m2.data[2+1*3];
	m1.data[2+2*3]+=m2.data[2+2*3];
}

void Math::sub(Matrix3x3 &r,const Matrix3x3 &m1,const Matrix3x3 &m2){
	r.data[0+0*3]=m1.data[0+0*3] - m2.data[0+0*3];
	r.data[0+1*3]=m1.data[0+1*3] - m2.data[0+1*3];
	r.data[0+2*3]=m1.data[0+2*3] - m2.data[0+2*3];

	r.data[1+0*3]=m1.data[1+0*3] - m2.data[1+0*3];
	r.data[1+1*3]=m1.data[1+1*3] - m2.data[1+1*3];
	r.data[1+2*3]=m1.data[1+2*3] - m2.data[1+2*3];

	r.data[2+0*3]=m1.data[2+0*3] - m2.data[2+0*3];
	r.data[2+1*3]=m1.data[2+1*3] - m2.data[2+1*3];
	r.data[2+2*3]=m1.data[2+2*3] - m2.data[2+2*3];
}

void Math::sub(Matrix3x3 &m1,const Matrix3x3 &m2){
	m1.data[0+0*3]-=m2.data[0+0*3];
	m1.data[0+1*3]-=m2.data[0+1*3];
	m1.data[0+2*3]-=m2.data[0+2*3];

	m1.data[1+0*3]-=m2.data[1+0*3];
	m1.data[1+1*3]-=m2.data[1+1*3];
	m1.data[1+2*3]-=m2.data[1+2*3];

	m1.data[2+0*3]-=m2.data[2+0*3];
	m1.data[2+1*3]-=m2.data[2+1*3];
	m1.data[2+2*3]-=m2.data[2+2*3];
}

void Math::mul(Matrix3x3 &r,const Matrix3x3 &m1,const Matrix3x3 &m2){
	r.data[0+0*3]=m1.data[0+0*3] * m2.data[0+0*3] + m1.data[0+1*3] * m2.data[1+0*3] + m1.data[0+2*3] * m2.data[2+0*3];
	r.data[0+1*3]=m1.data[0+0*3] * m2.data[0+1*3] + m1.data[0+1*3] * m2.data[1+1*3] + m1.data[0+2*3] * m2.data[2+1*3];
	r.data[0+2*3]=m1.data[0+0*3] * m2.data[0+2*3] + m1.data[0+1*3] * m2.data[1+2*3] + m1.data[0+2*3] * m2.data[2+2*3];

	r.data[1+0*3]=m1.data[1+0*3] * m2.data[0+0*3] + m1.data[1+1*3] * m2.data[1+0*3] + m1.data[1+2*3] * m2.data[2+0*3];
	r.data[1+1*3]=m1.data[1+0*3] * m2.data[0+1*3] + m1.data[1+1*3] * m2.data[1+1*3] + m1.data[1+2*3] * m2.data[2+1*3];
	r.data[1+2*3]=m1.data[1+0*3] * m2.data[0+2*3] + m1.data[1+1*3] * m2.data[1+2*3] + m1.data[1+2*3] * m2.data[2+2*3];

	r.data[2+0*3]=m1.data[2+0*3] * m2.data[0+0*3] + m1.data[2+1*3] * m2.data[1+0*3] + m1.data[2+2*3] * m2.data[2+0*3];
	r.data[2+1*3]=m1.data[2+0*3] * m2.data[0+1*3] + m1.data[2+1*3] * m2.data[1+1*3] + m1.data[2+2*3] * m2.data[2+1*3];
	r.data[2+2*3]=m1.data[2+0*3] * m2.data[0+2*3] + m1.data[2+1*3] * m2.data[1+2*3] + m1.data[2+2*3] * m2.data[2+2*3];
}

void Math::postMul(Matrix3x3 &m1,const Matrix3x3 &m2){
	real d00=m1.data[0+0*3] * m2.data[0+0*3] + m1.data[0+1*3] * m2.data[1+0*3] + m1.data[0+2*3] * m2.data[2+0*3];
	real d01=m1.data[0+0*3] * m2.data[0+1*3] + m1.data[0+1*3] * m2.data[1+1*3] + m1.data[0+2*3] * m2.data[2+1*3];
	real d02=m1.data[0+0*3] * m2.data[0+2*3] + m1.data[0+1*3] * m2.data[1+2*3] + m1.data[0+2*3] * m2.data[2+2*3];

	real d10=m1.data[1+0*3] * m2.data[0+0*3] + m1.data[1+1*3] * m2.data[1+0*3] + m1.data[1+2*3] * m2.data[2+0*3];
	real d11=m1.data[1+0*3] * m2.data[0+1*3] + m1.data[1+1*3] * m2.data[1+1*3] + m1.data[1+2*3] * m2.data[2+1*3];
	real d12=m1.data[1+0*3] * m2.data[0+2*3] + m1.data[1+1*3] * m2.data[1+2*3] + m1.data[1+2*3] * m2.data[2+2*3];

	real d20=m1.data[2+0*3] * m2.data[0+0*3] + m1.data[2+1*3] * m2.data[1+0*3] + m1.data[2+2*3] * m2.data[2+0*3];
	real d21=m1.data[2+0*3] * m2.data[0+1*3] + m1.data[2+1*3] * m2.data[1+1*3] + m1.data[2+2*3] * m2.data[2+1*3];
	real d22=m1.data[2+0*3] * m2.data[0+2*3] + m1.data[2+1*3] * m2.data[1+2*3] + m1.data[2+2*3] * m2.data[2+2*3];

	m1.data[0+0*3]=d00; m1.data[0+1*3]=d01; m1.data[0+2*3]=d02;
	m1.data[1+0*3]=d10; m1.data[1+1*3]=d11; m1.data[1+2*3]=d12;
	m1.data[2+0*3]=d20; m1.data[2+1*3]=d21; m1.data[2+2*3]=d22;
}

void Math::preMul(Matrix3x3 &m2,const Matrix3x3 &m1){
	real d00=m1.data[0+0*3] * m2.data[0+0*3] + m1.data[0+1*3] * m2.data[1+0*3] + m1.data[0+2*3] * m2.data[2+0*3];
	real d01=m1.data[0+0*3] * m2.data[0+1*3] + m1.data[0+1*3] * m2.data[1+1*3] + m1.data[0+2*3] * m2.data[2+1*3];
	real d02=m1.data[0+0*3] * m2.data[0+2*3] + m1.data[0+1*3] * m2.data[1+2*3] + m1.data[0+2*3] * m2.data[2+2*3];

	real d10=m1.data[1+0*3] * m2.data[0+0*3] + m1.data[1+1*3] * m2.data[1+0*3] + m1.data[1+2*3] * m2.data[2+0*3];
	real d11=m1.data[1+0*3] * m2.data[0+1*3] + m1.data[1+1*3] * m2.data[1+1*3] + m1.data[1+2*3] * m2.data[2+1*3];
	real d12=m1.data[1+0*3] * m2.data[0+2*3] + m1.data[1+1*3] * m2.data[1+2*3] + m1.data[1+2*3] * m2.data[2+2*3];

	real d20=m1.data[2+0*3] * m2.data[0+0*3] + m1.data[2+1*3] * m2.data[1+0*3] + m1.data[2+2*3] * m2.data[2+0*3];
	real d21=m1.data[2+0*3] * m2.data[0+1*3] + m1.data[2+1*3] * m2.data[1+1*3] + m1.data[2+2*3] * m2.data[2+1*3];
	real d22=m1.data[2+0*3] * m2.data[0+2*3] + m1.data[2+1*3] * m2.data[1+2*3] + m1.data[2+2*3] * m2.data[2+2*3];

	m2.data[0+0*3]=d00; m2.data[0+1*3]=d01; m2.data[0+2*3]=d02;
	m2.data[1+0*3]=d10; m2.data[1+1*3]=d11; m2.data[1+2*3]=d12;
	m2.data[2+0*3]=d20; m2.data[2+1*3]=d21; m2.data[2+2*3]=d22;
}

void Math::add(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
	r.data[0+0*4]=m1.data[0+0*4] + m2.data[0+0*4];
	r.data[0+1*4]=m1.data[0+1*4] + m2.data[0+1*4];
	r.data[0+2*4]=m1.data[0+2*4] + m2.data[0+2*4];
	r.data[0+3*4]=m1.data[0+3*4] + m2.data[0+3*4];

	r.data[1+0*4]=m1.data[1+0*4] + m2.data[1+0*4];
	r.data[1+1*4]=m1.data[1+1*4] + m2.data[1+1*4];
	r.data[1+2*4]=m1.data[1+2*4] + m2.data[1+2*4];
	r.data[1+3*4]=m1.data[1+3*4] + m2.data[1+3*4];

	r.data[2+0*4]=m1.data[2+0*4] + m2.data[2+0*4];
	r.data[2+1*4]=m1.data[2+1*4] + m2.data[2+1*4];
	r.data[2+2*4]=m1.data[2+2*4] + m2.data[2+2*4];
	r.data[2+3*4]=m1.data[2+3*4] + m2.data[2+3*4];

	r.data[3+0*4]=m1.data[3+0*4] + m2.data[3+0*4];
	r.data[3+1*4]=m1.data[3+1*4] + m2.data[3+1*4];
	r.data[3+2*4]=m1.data[3+2*4] + m2.data[3+2*4];
	r.data[3+3*4]=m1.data[3+3*4] + m2.data[3+3*4];
}

void Math::add(Matrix4x4 &m1,const Matrix4x4 &m2){
	m1.data[0+0*4]+=m2.data[0+0*4];
	m1.data[0+1*4]+=m2.data[0+1*4];
	m1.data[0+2*4]+=m2.data[0+2*4];
	m1.data[0+3*4]+=m2.data[0+3*4];

	m1.data[1+0*4]+=m2.data[1+0*4];
	m1.data[1+1*4]+=m2.data[1+1*4];
	m1.data[1+2*4]+=m2.data[1+2*4];
	m1.data[1+3*4]+=m2.data[1+3*4];

	m1.data[2+0*4]+=m2.data[2+0*4];
	m1.data[2+1*4]+=m2.data[2+1*4];
	m1.data[2+2*4]+=m2.data[2+2*4];
	m1.data[2+3*4]+=m2.data[2+3*4];

	m1.data[3+0*4]+=m2.data[3+0*4];
	m1.data[3+1*4]+=m2.data[3+1*4];
	m1.data[3+2*4]+=m2.data[3+2*4];
	m1.data[3+3*4]+=m2.data[3+3*4];
}

void Math::sub(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
	r.data[0+0*4]=m1.data[0+0*4] - m2.data[0+0*4];
	r.data[0+1*4]=m1.data[0+1*4] - m2.data[0+1*4];
	r.data[0+2*4]=m1.data[0+2*4] - m2.data[0+2*4];
	r.data[0+3*4]=m1.data[0+3*4] - m2.data[0+3*4];

	r.data[1+0*4]=m1.data[1+0*4] - m2.data[1+0*4];
	r.data[1+1*4]=m1.data[1+1*4] - m2.data[1+1*4];
	r.data[1+2*4]=m1.data[1+2*4] - m2.data[1+2*4];
	r.data[1+3*4]=m1.data[1+3*4] - m2.data[1+3*4];

	r.data[2+0*4]=m1.data[2+0*4] - m2.data[2+0*4];
	r.data[2+1*4]=m1.data[2+1*4] - m2.data[2+1*4];
	r.data[2+2*4]=m1.data[2+2*4] - m2.data[2+2*4];
	r.data[2+3*4]=m1.data[2+3*4] - m2.data[2+3*4];

	r.data[3+0*4]=m1.data[3+0*4] - m2.data[3+0*4];
	r.data[3+1*4]=m1.data[3+1*4] - m2.data[3+1*4];
	r.data[3+2*4]=m1.data[3+2*4] - m2.data[3+2*4];
	r.data[3+3*4]=m1.data[3+3*4] - m2.data[3+3*4];
}

void Math::sub(Matrix4x4 &m1,const Matrix4x4 &m2){
	m1.data[0+0*4]-=m2.data[0+0*4];
	m1.data[0+1*4]-=m2.data[0+1*4];
	m1.data[0+2*4]-=m2.data[0+2*4];
	m1.data[0+3*4]-=m2.data[0+3*4];

	m1.data[1+0*4]-=m2.data[1+0*4];
	m1.data[1+1*4]-=m2.data[1+1*4];
	m1.data[1+2*4]-=m2.data[1+2*4];
	m1.data[1+3*4]-=m2.data[1+3*4];

	m1.data[2+0*4]-=m2.data[2+0*4];
	m1.data[2+1*4]-=m2.data[2+1*4];
	m1.data[2+2*4]-=m2.data[2+2*4];
	m1.data[2+3*4]-=m2.data[2+3*4];

	m1.data[3+0*4]-=m2.data[3+0*4];
	m1.data[3+1*4]-=m2.data[3+1*4];
	m1.data[3+2*4]-=m2.data[3+2*4];
	m1.data[3+3*4]-=m2.data[3+3*4];
}

void(*mulMatrix4x4)(Matrix4x4 &,const Matrix4x4 &,const Matrix4x4 &);
void Math::mul(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){(*mulMatrix4x4)(r,m1,m2);}

void(*postMulMatrix4x4)(Matrix4x4 &,const Matrix4x4 &);
void Math::postMul(Matrix4x4 &m1,const Matrix4x4 &m2){postMulMatrix4x4(m1,m2);}

void(*preMulMatrix4x4)(Matrix4x4 &,const Matrix4x4 &);
void Math::preMul(Matrix4x4 &m2,const Matrix4x4 &m1){preMulMatrix4x4(m2,m1);}

void(*mulVector4Matrix4x4Vector4)(Vector4 &,const Matrix4x4 &,const Vector4 &);
void Math::mul(Vector4 &r,const Matrix4x4 &m,const Vector4 &v){mulVector4Matrix4x4Vector4(r,m,v);}

void(*mulVector4Matrix4x4)(Vector4 &,const Matrix4x4 &);
void Math::mul(Vector4 &r,const Matrix4x4 &m){mulVector4Matrix4x4(r,m);}

void Math::mulMatrix4x4Traditional(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
	r.data[0+0*4]=(m1.data[0+0*4]*m2.data[0+0*4]) + (m1.data[0+1*4]*m2.data[1+0*4]) + (m1.data[0+2*4]*m2.data[2+0*4]) + (m1.data[0+3*4]*m2.data[3+0*4]);
	r.data[0+1*4]=(m1.data[0+0*4]*m2.data[0+1*4]) + (m1.data[0+1*4]*m2.data[1+1*4]) + (m1.data[0+2*4]*m2.data[2+1*4]) + (m1.data[0+3*4]*m2.data[3+1*4]);
	r.data[0+2*4]=(m1.data[0+0*4]*m2.data[0+2*4]) + (m1.data[0+1*4]*m2.data[1+2*4]) + (m1.data[0+2*4]*m2.data[2+2*4]) + (m1.data[0+3*4]*m2.data[3+2*4]);
	r.data[0+3*4]=(m1.data[0+0*4]*m2.data[0+3*4]) + (m1.data[0+1*4]*m2.data[1+3*4]) + (m1.data[0+2*4]*m2.data[2+3*4]) + (m1.data[0+3*4]*m2.data[3+3*4]);

	r.data[1+0*4]=(m1.data[1+0*4]*m2.data[0+0*4]) + (m1.data[1+1*4]*m2.data[1+0*4]) + (m1.data[1+2*4]*m2.data[2+0*4]) + (m1.data[1+3*4]*m2.data[3+0*4]);
	r.data[1+1*4]=(m1.data[1+0*4]*m2.data[0+1*4]) + (m1.data[1+1*4]*m2.data[1+1*4]) + (m1.data[1+2*4]*m2.data[2+1*4]) + (m1.data[1+3*4]*m2.data[3+1*4]);
	r.data[1+2*4]=(m1.data[1+0*4]*m2.data[0+2*4]) + (m1.data[1+1*4]*m2.data[1+2*4]) + (m1.data[1+2*4]*m2.data[2+2*4]) + (m1.data[1+3*4]*m2.data[3+2*4]);
	r.data[1+3*4]=(m1.data[1+0*4]*m2.data[0+3*4]) + (m1.data[1+1*4]*m2.data[1+3*4]) + (m1.data[1+2*4]*m2.data[2+3*4]) + (m1.data[1+3*4]*m2.data[3+3*4]);

	r.data[2+0*4]=(m1.data[2+0*4]*m2.data[0+0*4]) + (m1.data[2+1*4]*m2.data[1+0*4]) + (m1.data[2+2*4]*m2.data[2+0*4]) + (m1.data[2+3*4]*m2.data[3+0*4]);
	r.data[2+1*4]=(m1.data[2+0*4]*m2.data[0+1*4]) + (m1.data[2+1*4]*m2.data[1+1*4]) + (m1.data[2+2*4]*m2.data[2+1*4]) + (m1.data[2+3*4]*m2.data[3+1*4]);
	r.data[2+2*4]=(m1.data[2+0*4]*m2.data[0+2*4]) + (m1.data[2+1*4]*m2.data[1+2*4]) + (m1.data[2+2*4]*m2.data[2+2*4]) + (m1.data[2+3*4]*m2.data[3+2*4]);
	r.data[2+3*4]=(m1.data[2+0*4]*m2.data[0+3*4]) + (m1.data[2+1*4]*m2.data[1+3*4]) + (m1.data[2+2*4]*m2.data[2+3*4]) + (m1.data[2+3*4]*m2.data[3+3*4]);

	r.data[3+0*4]=(m1.data[3+0*4]*m2.data[0+0*4]) + (m1.data[3+1*4]*m2.data[1+0*4]) + (m1.data[3+2*4]*m2.data[2+0*4]) + (m1.data[3+3*4]*m2.data[3+0*4]);
	r.data[3+1*4]=(m1.data[3+0*4]*m2.data[0+1*4]) + (m1.data[3+1*4]*m2.data[1+1*4]) + (m1.data[3+2*4]*m2.data[2+1*4]) + (m1.data[3+3*4]*m2.data[3+1*4]);
	r.data[3+2*4]=(m1.data[3+0*4]*m2.data[0+2*4]) + (m1.data[3+1*4]*m2.data[1+2*4]) + (m1.data[3+2*4]*m2.data[2+2*4]) + (m1.data[3+3*4]*m2.data[3+2*4]);
	r.data[3+3*4]=(m1.data[3+0*4]*m2.data[0+3*4]) + (m1.data[3+1*4]*m2.data[1+3*4]) + (m1.data[3+2*4]*m2.data[2+3*4]) + (m1.data[3+3*4]*m2.data[3+3*4]);
}

void Math::postMulMatrix4x4Traditional(Matrix4x4 &m1,const Matrix4x4 &m2){
	real d00=(m1.data[0+0*4]*m2.data[0+0*4]) + (m1.data[0+1*4]*m2.data[1+0*4]) + (m1.data[0+2*4]*m2.data[2+0*4]) + (m1.data[0+3*4]*m2.data[3+0*4]);
	real d01=(m1.data[0+0*4]*m2.data[0+1*4]) + (m1.data[0+1*4]*m2.data[1+1*4]) + (m1.data[0+2*4]*m2.data[2+1*4]) + (m1.data[0+3*4]*m2.data[3+1*4]);
	real d02=(m1.data[0+0*4]*m2.data[0+2*4]) + (m1.data[0+1*4]*m2.data[1+2*4]) + (m1.data[0+2*4]*m2.data[2+2*4]) + (m1.data[0+3*4]*m2.data[3+2*4]);
	real d03=(m1.data[0+0*4]*m2.data[0+3*4]) + (m1.data[0+1*4]*m2.data[1+3*4]) + (m1.data[0+2*4]*m2.data[2+3*4]) + (m1.data[0+3*4]*m2.data[3+3*4]);

	real d10=(m1.data[1+0*4]*m2.data[0+0*4]) + (m1.data[1+1*4]*m2.data[1+0*4]) + (m1.data[1+2*4]*m2.data[2+0*4]) + (m1.data[1+3*4]*m2.data[3+0*4]);
	real d11=(m1.data[1+0*4]*m2.data[0+1*4]) + (m1.data[1+1*4]*m2.data[1+1*4]) + (m1.data[1+2*4]*m2.data[2+1*4]) + (m1.data[1+3*4]*m2.data[3+1*4]);
	real d12=(m1.data[1+0*4]*m2.data[0+2*4]) + (m1.data[1+1*4]*m2.data[1+2*4]) + (m1.data[1+2*4]*m2.data[2+2*4]) + (m1.data[1+3*4]*m2.data[3+2*4]);
	real d13=(m1.data[1+0*4]*m2.data[0+3*4]) + (m1.data[1+1*4]*m2.data[1+3*4]) + (m1.data[1+2*4]*m2.data[2+3*4]) + (m1.data[1+3*4]*m2.data[3+3*4]);

	real d20=(m1.data[2+0*4]*m2.data[0+0*4]) + (m1.data[2+1*4]*m2.data[1+0*4]) + (m1.data[2+2*4]*m2.data[2+0*4]) + (m1.data[2+3*4]*m2.data[3+0*4]);
	real d21=(m1.data[2+0*4]*m2.data[0+1*4]) + (m1.data[2+1*4]*m2.data[1+1*4]) + (m1.data[2+2*4]*m2.data[2+1*4]) + (m1.data[2+3*4]*m2.data[3+1*4]);
	real d22=(m1.data[2+0*4]*m2.data[0+2*4]) + (m1.data[2+1*4]*m2.data[1+2*4]) + (m1.data[2+2*4]*m2.data[2+2*4]) + (m1.data[2+3*4]*m2.data[3+2*4]);
	real d23=(m1.data[2+0*4]*m2.data[0+3*4]) + (m1.data[2+1*4]*m2.data[1+3*4]) + (m1.data[2+2*4]*m2.data[2+3*4]) + (m1.data[2+3*4]*m2.data[3+3*4]);

	real d30=(m1.data[3+0*4]*m2.data[0+0*4]) + (m1.data[3+1*4]*m2.data[1+0*4]) + (m1.data[3+2*4]*m2.data[2+0*4]) + (m1.data[3+3*4]*m2.data[3+0*4]);
	real d31=(m1.data[3+0*4]*m2.data[0+1*4]) + (m1.data[3+1*4]*m2.data[1+1*4]) + (m1.data[3+2*4]*m2.data[2+1*4]) + (m1.data[3+3*4]*m2.data[3+1*4]);
	real d32=(m1.data[3+0*4]*m2.data[0+2*4]) + (m1.data[3+1*4]*m2.data[1+2*4]) + (m1.data[3+2*4]*m2.data[2+2*4]) + (m1.data[3+3*4]*m2.data[3+2*4]);
	real d33=(m1.data[3+0*4]*m2.data[0+3*4]) + (m1.data[3+1*4]*m2.data[1+3*4]) + (m1.data[3+2*4]*m2.data[2+3*4]) + (m1.data[3+3*4]*m2.data[3+3*4]);

	m1.data[0+0*4]=d00; m1.data[0+1*4]=d01; m1.data[0+2*4]=d02; m1.data[0+3*4]=d03;
	m1.data[1+0*4]=d10; m1.data[1+1*4]=d11; m1.data[1+2*4]=d12; m1.data[1+3*4]=d13;
	m1.data[2+0*4]=d20; m1.data[2+1*4]=d21; m1.data[2+2*4]=d22; m1.data[2+3*4]=d23;
	m1.data[3+0*4]=d30; m1.data[3+1*4]=d31; m1.data[3+2*4]=d32; m1.data[3+3*4]=d33;
}

void Math::preMulMatrix4x4Traditional(Matrix4x4 &m2,const Matrix4x4 &m1){
	real d00=(m1.data[0+0*4]*m2.data[0+0*4]) + (m1.data[0+1*4]*m2.data[1+0*4]) + (m1.data[0+2*4]*m2.data[2+0*4]) + (m1.data[0+3*4]*m2.data[3+0*4]);
	real d01=(m1.data[0+0*4]*m2.data[0+1*4]) + (m1.data[0+1*4]*m2.data[1+1*4]) + (m1.data[0+2*4]*m2.data[2+1*4]) + (m1.data[0+3*4]*m2.data[3+1*4]);
	real d02=(m1.data[0+0*4]*m2.data[0+2*4]) + (m1.data[0+1*4]*m2.data[1+2*4]) + (m1.data[0+2*4]*m2.data[2+2*4]) + (m1.data[0+3*4]*m2.data[3+2*4]);
	real d03=(m1.data[0+0*4]*m2.data[0+3*4]) + (m1.data[0+1*4]*m2.data[1+3*4]) + (m1.data[0+2*4]*m2.data[2+3*4]) + (m1.data[0+3*4]*m2.data[3+3*4]);

	real d10=(m1.data[1+0*4]*m2.data[0+0*4]) + (m1.data[1+1*4]*m2.data[1+0*4]) + (m1.data[1+2*4]*m2.data[2+0*4]) + (m1.data[1+3*4]*m2.data[3+0*4]);
	real d11=(m1.data[1+0*4]*m2.data[0+1*4]) + (m1.data[1+1*4]*m2.data[1+1*4]) + (m1.data[1+2*4]*m2.data[2+1*4]) + (m1.data[1+3*4]*m2.data[3+1*4]);
	real d12=(m1.data[1+0*4]*m2.data[0+2*4]) + (m1.data[1+1*4]*m2.data[1+2*4]) + (m1.data[1+2*4]*m2.data[2+2*4]) + (m1.data[1+3*4]*m2.data[3+2*4]);
	real d13=(m1.data[1+0*4]*m2.data[0+3*4]) + (m1.data[1+1*4]*m2.data[1+3*4]) + (m1.data[1+2*4]*m2.data[2+3*4]) + (m1.data[1+3*4]*m2.data[3+3*4]);

	real d20=(m1.data[2+0*4]*m2.data[0+0*4]) + (m1.data[2+1*4]*m2.data[1+0*4]) + (m1.data[2+2*4]*m2.data[2+0*4]) + (m1.data[2+3*4]*m2.data[3+0*4]);
	real d21=(m1.data[2+0*4]*m2.data[0+1*4]) + (m1.data[2+1*4]*m2.data[1+1*4]) + (m1.data[2+2*4]*m2.data[2+1*4]) + (m1.data[2+3*4]*m2.data[3+1*4]);
	real d22=(m1.data[2+0*4]*m2.data[0+2*4]) + (m1.data[2+1*4]*m2.data[1+2*4]) + (m1.data[2+2*4]*m2.data[2+2*4]) + (m1.data[2+3*4]*m2.data[3+2*4]);
	real d23=(m1.data[2+0*4]*m2.data[0+3*4]) + (m1.data[2+1*4]*m2.data[1+3*4]) + (m1.data[2+2*4]*m2.data[2+3*4]) + (m1.data[2+3*4]*m2.data[3+3*4]);

	real d30=(m1.data[3+0*4]*m2.data[0+0*4]) + (m1.data[3+1*4]*m2.data[1+0*4]) + (m1.data[3+2*4]*m2.data[2+0*4]) + (m1.data[3+3*4]*m2.data[3+0*4]);
	real d31=(m1.data[3+0*4]*m2.data[0+1*4]) + (m1.data[3+1*4]*m2.data[1+1*4]) + (m1.data[3+2*4]*m2.data[2+1*4]) + (m1.data[3+3*4]*m2.data[3+1*4]);
	real d32=(m1.data[3+0*4]*m2.data[0+2*4]) + (m1.data[3+1*4]*m2.data[1+2*4]) + (m1.data[3+2*4]*m2.data[2+2*4]) + (m1.data[3+3*4]*m2.data[3+2*4]);
	real d33=(m1.data[3+0*4]*m2.data[0+3*4]) + (m1.data[3+1*4]*m2.data[1+3*4]) + (m1.data[3+2*4]*m2.data[2+3*4]) + (m1.data[3+3*4]*m2.data[3+3*4]);

	m2.data[0+0*4]=d00; m2.data[0+1*4]=d01; m2.data[0+2*4]=d02; m2.data[0+3*4]=d03;
	m2.data[1+0*4]=d10; m2.data[1+1*4]=d11; m2.data[1+2*4]=d12; m2.data[1+3*4]=d13;
	m2.data[2+0*4]=d20; m2.data[2+1*4]=d21; m2.data[2+2*4]=d22; m2.data[2+3*4]=d23;
	m2.data[3+0*4]=d30; m2.data[3+1*4]=d31; m2.data[3+2*4]=d32; m2.data[3+3*4]=d33;
}

void Math::mulVector4Matrix4x4Vector4Traditional(Vector4 &r,const Matrix4x4 &m,const Vector4 &v){
	r.x=(m.data[0+0*4]*v.x) + (m.data[0+1*4]*v.y) + (m.data[0+2*4]*v.z) + (m.data[0+3*4]*v.w);
	r.y=(m.data[1+0*4]*v.x) + (m.data[1+1*4]*v.y) + (m.data[1+2*4]*v.z) + (m.data[1+3*4]*v.w);
	r.z=(m.data[2+0*4]*v.x) + (m.data[2+1*4]*v.y) + (m.data[2+2*4]*v.z) + (m.data[2+3*4]*v.w);
	r.w=(m.data[3+0*4]*v.x) + (m.data[3+1*4]*v.y) + (m.data[3+2*4]*v.z) + (m.data[3+3*4]*v.w);
}

void Math::mulVector4Matrix4x4Traditional(Vector4 &r,const Matrix4x4 &m){
	real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z) + (m.data[0+3*4]*r.w);
	real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z) + (m.data[1+3*4]*r.w);
	real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z) + (m.data[2+3*4]*r.w);
	real tw=(m.data[3+0*4]*r.x) + (m.data[3+1*4]*r.y) + (m.data[3+2*4]*r.z) + (m.data[3+3*4]*r.w);
	r.x=tx;
	r.y=ty;
	r.z=tz;
	r.w=tw;
}

// Algorithm from the GMTL: http://ggt.sf.net
template<class Matrix>
bool setMatrixFromEulerAngle(Matrix &r,const EulerAngle &a,real epsilon){
	const real x=(a.order==EulerAngle::EulerOrder_XYZ)?a.x:((a.order==EulerAngle::EulerOrder_ZXY)?a.y:a.z);
	const real y=(a.order==EulerAngle::EulerOrder_XYZ)?a.y:((a.order==EulerAngle::EulerOrder_ZXY)?a.z:a.y);
	const real z=(a.order==EulerAngle::EulerOrder_XYZ)?a.z:((a.order==EulerAngle::EulerOrder_ZXY)?a.x:a.x);

	real sx=sin(x),cx=cos(x);
	real sy=sin(y),cy=cos(y);
	real sz=sin(z),cz=cos(z);

	switch(a.order){
		case EulerAngle::EulerOrder_XYZ:
			r.setAt(0,0,cy*cz);
			r.setAt(0,1,-cy*sz);
			r.setAt(0,2,sy);
			r.setAt(1,0,sx*sy*cz + cx*sz);
			r.setAt(1,1,-sx*sy*sz + cx*cz);
			r.setAt(1,2,-sx*cy);
			r.setAt(2,0,-cx*sy*cz + sx*sz);
			r.setAt(2,1,cx*sy*sz + sx*cz);
			r.setAt(2,2,cx*cy);
		break;
		case EulerAngle::EulerOrder_ZYX:
			r.setAt(0,0,cy*cz);
			r.setAt(0,1,-cx*sz + sx*sy*cz);
			r.setAt(0,2,sx*sz + cx*sy*cz);
			r.setAt(1,0,cy*sz);
			r.setAt(1,1,cx*cz + sx*sy*sz);
			r.setAt(1,2,-sx*cz + cx*sy*sz);
			r.setAt(2,0,-sy);
			r.setAt(2,1,sx*cy);
			r.setAt(2,2,cx*cy);
		break;
		case EulerAngle::EulerOrder_ZXY:
			r.setAt(0,0,cy*cz - sx*sy*sz);
			r.setAt(0,1,-cx*sz);
			r.setAt(0,2,sy*cz + sx*cy*sz);
			r.setAt(1,0,cy*sz + sx*sy*cz);
			r.setAt(1,1,cx*cz);
			r.setAt(1,2,sy*sz - sx*cy*cz);
			r.setAt(2,0,-cx*sy);
			r.setAt(2,1,sx);
			r.setAt(2,2,cx*cy);
		break;
		default:
			return false;
	}

	return true;
}

void Math::setMatrix3x3FromEulerAngle(Matrix3x3 &r,const EulerAngle &a,real epsilon){
	setMatrixFromEulerAngle(r,a,epsilon);
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

bool Math::invert(Matrix3x3 &r,const Matrix3x3 &m){
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

void Math::setMatrix4x4FromEulerAngle(Matrix4x4 &r,const EulerAngle &a,real epsilon){
	setMatrixFromEulerAngle(r,a,epsilon);
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
	r.data[14] = -(r.data[2]*eye.x + r.data[6]*eye.y + r.data[10]*eye.z);
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

void Math::setQuaternionFromMatrix3x3(Quaternion &r,const Matrix3x3 &m){
	Vector3 scale;
	setScaleFromMatrix3x3(scale,m);
	Matrix3x3 m2;
	m2.setAt(0,0,m.at(0,0)/scale.x);
	m2.setAt(1,0,m.at(1,0)/scale.x);
	m2.setAt(2,0,m.at(2,0)/scale.x);
	m2.setAt(0,1,m.at(0,1)/scale.y);
	m2.setAt(1,1,m.at(1,1)/scale.y);
	m2.setAt(2,1,m.at(2,1)/scale.y);
	m2.setAt(0,2,m.at(0,2)/scale.z);
	m2.setAt(1,2,m.at(1,2)/scale.z);
	m2.setAt(2,2,m.at(2,2)/scale.z);
	setQuaternionFromMatrix(r,m2);
}

void Math::setQuaternionFromMatrix4x4(Quaternion &r,const Matrix4x4 &m){
	Vector3 scale;
	setScaleFromMatrix4x4(scale,m);
	Matrix4x4 m2(m);
	m2.setAt(0,0,m.at(0,0)/scale.x);
	m2.setAt(1,0,m.at(1,0)/scale.x);
	m2.setAt(2,0,m.at(2,0)/scale.x);
	m2.setAt(0,1,m.at(0,1)/scale.y);
	m2.setAt(1,1,m.at(1,1)/scale.y);
	m2.setAt(2,1,m.at(2,1)/scale.y);
	m2.setAt(0,2,m.at(0,2)/scale.z);
	m2.setAt(1,2,m.at(1,2)/scale.z);
	m2.setAt(2,2,m.at(2,2)/scale.z);
	setQuaternionFromMatrix(r,m2);
}

inline void setQuaternionsFromAngles(Quaternion &qx,Quaternion &qy,Quaternion &qz,real x,real y,real z){
	real hx=x*0.5;
	real hy=y*0.5;
	real hz=z*0.5;

	qx.x=sin(hx);
	qx.w=cos(hx);

	qy.y=sin(hy);
	qy.w=cos(hy);

	qz.z=sin(hz);
	qz.w=cos(hz);
}

// Algorithm from the GMTL: http://ggt.sf.net
bool Math::setQuaternionFromEulerAngle(Quaternion &r,const EulerAngle &a){
	Quaternion qx,qy,qz;

	switch(a.order){
		case EulerAngle::EulerOrder_XYZ:
			setQuaternionsFromAngles(qx,qy,qz,a.x,a.y,a.z);
			r=qx*qy*qz;
		break;
		case EulerAngle::EulerOrder_ZYX:
			setQuaternionsFromAngles(qx,qy,qz,a.z,a.y,a.x);
			r=qz*qy*qx;
		break;
		case EulerAngle::EulerOrder_ZXY:
			setQuaternionsFromAngles(qx,qy,qz,a.y,a.z,a.x);
			r=qz*qx*qy;
		break;
		default:
			return false;
	}

	return true;
}

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

// Algorithm from the GMTL: http://ggt.sf.net
template<class Matrix>
bool setEulerAngleFromMatrix(EulerAngle &r,const Matrix &m,real epsilon){
	real x,y,z;
	switch(r.order){
		case EulerAngle::EulerOrder_XYZ:
			y=Math::asin(m.at(0,2));
			if(y<Math::HALF_PI){
				if(y>-Math::HALF_PI){
					x=Math::atan2(-m.at(1,2),m.at(2,2));
					z=Math::atan2(-m.at(0,1),m.at(0,0));
				}
				else{
					x=-Math::atan2(m.at(1,0),m.at(1,1));
					z=0;
				}
			}
			else{
				x=-Math::atan2(m.at(1,0),m.at(1,1));
				z=0;
			}
			r.x=x;
			r.y=y;
			r.z=z;
		break;
		case EulerAngle::EulerOrder_ZYX:
			y=Math::asin(-m.at(2,0));
			if(y<Math::HALF_PI){
				if(y>-Math::HALF_PI){
					z=Math::atan2(m.at(1,0),m.at(0,0));
					x=Math::atan2(m.at(2,1),m.at(2,2));
				}
				else{
					z=Math::atan2(-m.at(0,1),-m.at(0,2));
					x=0;
				}
			}
			else{
				z=-atan2(m.at(0,1),m.at(0,2));
				x=0;
			}
			r.x=z;
			r.y=y;
			r.z=x;
		break;
		case EulerAngle::EulerOrder_ZXY:
			x=Math::asin(m.at(2,1));
			if(x<Math::HALF_PI){
				if(x>-Math::HALF_PI){
					z=Math::atan2(-m.at(0,1),m.at(1,1));
					y=Math::atan2(-m.at(2,0),m.at(2,2));
				}
				else{
					z=-Math::atan2(m.at(0,2),m.at(0,0));
					y=0;
				}
			}
			else{
				z=Math::atan2(m.at(0,2),m.at(0,0));
				y=0;
			}
			r.x=z;
			r.y=x;
			r.z=y;
		break;
		default:
			return false;
	}

	return true;
}

bool Math::setEulerAngleFromMatrix3x3(EulerAngle &r,const Matrix3x3 &m,real epsilon){
	return setEulerAngleFromMatrix(r,m,epsilon);
}

bool Math::setEulerAngleFromMatrix4x4(EulerAngle &r,const Matrix3x3 &m,real epsilon){
	return setEulerAngleFromMatrix(r,m,epsilon);
}

bool Math::setEulerAngleFromQuaternion(EulerAngle &r,const Quaternion &q,real epsilon){
	Matrix3x3 matrix;
	setMatrix3x3FromQuaternion(matrix,q);
	return setEulerAngleFromMatrix3x3(r,matrix,epsilon);
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

int Math::setClipPlanesFromProjectionMatrix(Plane *planes,int maxNumPlanes,const Matrix4x4 &m){
	const real *d=m.data;

	// Right clipping plane.
	normalize(planes[0].set(d[3]-d[0], d[7]-d[4], d[11]-d[8], d[15]-d[12]));
	// Left clipping plane.
	normalize(planes[1].set(d[3]+d[0], d[7]+d[4], d[11]+d[8], d[15]+d[12]));
	// Bottom clipping plane.
	normalize(planes[2].set(d[3]+d[1], d[7]+d[5], d[11]+d[9], d[15]+d[13]));
	// Top clipping plane.
	normalize(planes[3].set(d[3]-d[1], d[7]-d[5], d[11]-d[9], d[15]-d[13]));
	// Far clipping plane.
	normalize(planes[4].set(d[3]-d[2], d[7]-d[6], d[11]-d[10], d[15]-d[14]));
	// Near clipping plane.
	normalize(planes[5].set(d[3]+d[2], d[7]+d[6], d[11]+d[10], d[15]+d[14]));

	return 6;
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
	real d=dot(plane1.normal,plane2.normal);
	real determinant=lengthSquared(plane1.normal)*lengthSquared(plane2.normal) - (d*d);

	real c1=(plane1.distance*lengthSquared(plane2.normal) - plane2.distance*d)/determinant;
	real c2=(plane2.distance*lengthSquared(plane1.normal) - plane1.distance*d)/determinant;

	cross(result.direction,plane1.normal,plane2.normal);
	Vector3 n1,n2;
	mul(n1,plane1.normal,c1);
	mul(n2,plane2.normal,c2);
	add(result.origin,n1,n2);
}

void Math::findBoundingBox(AABox &r,const Sphere &sphere){
	real radius=sphere.radius;
	r.mins.x=-radius;
	r.mins.y=-radius;
	r.mins.z=-radius;
	r.maxs.x=radius;
	r.maxs.y=radius;
	r.maxs.z=radius;
	Math::add(r,sphere.origin);
}

void Math::findBoundingBox(AABox &r,const Capsule &capsule){
	real radius=capsule.radius;
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

bool Math::testIntersection(const Sphere &sphere,const Plane *planes,int numPlanes){
	if(sphere.radius<0) return true;
	real distance=0;
	int i;
	for(i=0;i<numPlanes;++i){
		distance=Math::dot(planes[i].normal,sphere.origin)+planes[i].distance;
		if(distance<-sphere.radius){
			return false;
		}
	}
	return true;
}

bool Math::testIntersection(const AABox &box,const Plane *planes,int numPlanes){
	int i;
	for(i=0;i<numPlanes;i++){
		if(box.findPVertexLength(planes[i])<0){
			return false;
		}
	}
	return true;
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

#if defined(TOADLET_HAS_SSE)

void Math::mulMatrix4x4SSE(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
	TOADLET_ASSERT(TOADLET_IS_ALIGNED(r) && TOADLET_IS_ALIGNED(m1) && TOADLET_IS_ALIGNED(m2));

	__m128 mm1[4],mtl,mrl;
	mm1[0]=_mm_load_ps(m1.data+0);
	mm1[1]=_mm_load_ps(m1.data+4);
	mm1[2]=_mm_load_ps(m1.data+8);
	mm1[3]=_mm_load_ps(m1.data+12);

	mtl=_mm_set1_ps(m2.data[0]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[1]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[2]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[3]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(r.data+0,mrl);

	mtl=_mm_set1_ps(m2.data[4]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[5]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[6]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[7]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(r.data+4,mrl);

	mtl=_mm_set1_ps(m2.data[8]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[9]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[10]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[11]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(r.data+8,mrl);

	mtl=_mm_set1_ps(m2.data[12]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[13]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[14]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[15]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(r.data+12,mrl);
}

void Math::postMulMatrix4x4SSE(Matrix4x4 &m1,const Matrix4x4 &m2){
	TOADLET_ASSERT(TOADLET_IS_ALIGNED(m1) && TOADLET_IS_ALIGNED(m2));

	__m128 mm1[4],mtl,mrl;
	mm1[0]=_mm_load_ps(m1.data+0);
	mm1[1]=_mm_load_ps(m1.data+4);
	mm1[2]=_mm_load_ps(m1.data+8);
	mm1[3]=_mm_load_ps(m1.data+12);

	mtl=_mm_set1_ps(m2.data[0]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[1]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[2]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[3]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m1.data+0,mrl);

	mtl=_mm_set1_ps(m2.data[4]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[5]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[6]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[7]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m1.data+4,mrl);

	mtl=_mm_set1_ps(m2.data[8]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[9]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[10]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[11]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m1.data+8,mrl);

	mtl=_mm_set1_ps(m2.data[12]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[13]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[14]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[15]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m1.data+12,mrl);
}

void Math::preMulMatrix4x4SSE(Matrix4x4 &m2,const Matrix4x4 &m1){
	TOADLET_ASSERT(TOADLET_IS_ALIGNED(m2) && TOADLET_IS_ALIGNED(m1));

	__m128 mm1[4],mtl,mrl;
	mm1[0]=_mm_load_ps(m1.data+0);
	mm1[1]=_mm_load_ps(m1.data+4);
	mm1[2]=_mm_load_ps(m1.data+8);
	mm1[3]=_mm_load_ps(m1.data+12);

	mtl=_mm_set1_ps(m2.data[0]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[1]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[2]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[3]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m2.data+0,mrl);

	mtl=_mm_set1_ps(m2.data[4]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[5]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[6]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[7]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m2.data+4,mrl);

	mtl=_mm_set1_ps(m2.data[8]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[9]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[10]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[11]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m2.data+8,mrl);

	mtl=_mm_set1_ps(m2.data[12]);
	mrl=_mm_mul_ps(mm1[0],mtl);
	mtl=_mm_set1_ps(m2.data[13]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[1],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[14]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[2],mtl),mrl);
	mtl=_mm_set1_ps(m2.data[15]);
	mrl=_mm_add_ps(_mm_mul_ps(mm1[3],mtl),mrl);
	_mm_store_ps(m2.data+12,mrl);
}

void Math::mulVector4Matrix4x4Vector4SSE(Vector4 &r,const Matrix4x4 &m,const Vector4 &v){
	TOADLET_ASSERT(TOADLET_IS_ALIGNED(r) && TOADLET_IS_ALIGNED(m) && TOADLET_IS_ALIGNED(v));

	__m128 mml,mtl,mrl;

	mml=_mm_load_ps(m.data+0);
	mtl=_mm_set1_ps(v.x);
	mrl=_mm_mul_ps(mml,mtl);
	mml=_mm_load_ps(m.data+4);
	mtl=_mm_set1_ps(v.y);
	mrl=_mm_add_ps(_mm_mul_ps(mml,mtl),mrl);
	mml=_mm_load_ps(m.data+8);
	mtl=_mm_set1_ps(v.z);
	mrl=_mm_add_ps(_mm_mul_ps(mml,mtl),mrl);
	mml=_mm_load_ps(m.data+12);
	mtl=_mm_set1_ps(v.w);
	mrl=_mm_add_ps(_mm_mul_ps(mml,mtl),mrl);
	_mm_store_ps((float*)&r,mrl);
}

void Math::mulVector4Matrix4x4SSE(Vector4 &r,const Matrix4x4 &m){
	TOADLET_ASSERT(TOADLET_IS_ALIGNED(r) && TOADLET_IS_ALIGNED(m));

	__m128 mml,mtl,mrl;

	mml=_mm_load_ps(m.data+0);
	mtl=_mm_set1_ps(r.x);
	mrl=_mm_mul_ps(mml,mtl);
	mml=_mm_load_ps(m.data+4);
	mtl=_mm_set1_ps(r.y);
	mrl=_mm_add_ps(_mm_mul_ps(mml,mtl),mrl);
	mml=_mm_load_ps(m.data+8);
	mtl=_mm_set1_ps(r.z);
	mrl=_mm_add_ps(_mm_mul_ps(mml,mtl),mrl);
	mml=_mm_load_ps(m.data+12);
	mtl=_mm_set1_ps(r.w);
	mrl=_mm_add_ps(_mm_mul_ps(mml,mtl),mrl);
	_mm_store_ps((float*)&r,mrl);
}

#elif defined(TOADLET_HAS_NEON)

void Math::mulMatrix4x4NEON(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
	asm volatile(
		"vldmia %1, {q4-q7}\n"
		"vldmia %2, {q8-q11}\n"

		"vmul.f32 q0, q8, d8[0]\n"
		"vmul.f32 q1, q8, d10[0]\n"
		"vmul.f32 q2, q8, d12[0]\n"
		"vmul.f32 q3, q8, d14[0]\n"

		"vmla.f32 q0, q9, d8[1]\n"
		"vmla.f32 q1, q9, d10[1]\n"
		"vmla.f32 q2, q9, d12[1]\n"
		"vmla.f32 q3, q9, d14[1]\n"

		"vmla.f32 q0, q10, d9[0]\n"
		"vmla.f32 q1, q10, d11[0]\n"
		"vmla.f32 q2, q10, d13[0]\n"
		"vmla.f32 q3, q10, d15[0]\n"

		"vmla.f32 q0, q11, d9[1]\n"
		"vmla.f32 q1, q11, d11[1]\n"
		"vmla.f32 q2, q11, d13[1]\n"
		"vmla.f32 q3, q11, d15[1]\n"

		"vstmia %0, {q0-q3}"
		:
		: "r" (r.getData()), "r" (m1.getData()), "r" (m2.getData())
		: "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q11"
	);
}

void Math::postMulMatrix4x4NEON(Matrix4x4 &m1,const Matrix4x4 &m2){
	asm volatile(
		"vldmia %1, {q4-q7}\n"
		"vldmia %2, {q8-q11}\n"

		"vmul.f32 q0, q8, d8[0]\n"
		"vmul.f32 q1, q8, d10[0]\n"
		"vmul.f32 q2, q8, d12[0]\n"
		"vmul.f32 q3, q8, d14[0]\n"

		"vmla.f32 q0, q9, d8[1]\n"
		"vmla.f32 q1, q9, d10[1]\n"
		"vmla.f32 q2, q9, d12[1]\n"
		"vmla.f32 q3, q9, d14[1]\n"

		"vmla.f32 q0, q10, d9[0]\n"
		"vmla.f32 q1, q10, d11[0]\n"
		"vmla.f32 q2, q10, d13[0]\n"
		"vmla.f32 q3, q10, d15[0]\n"

		"vmla.f32 q0, q11, d9[1]\n"
		"vmla.f32 q1, q11, d11[1]\n"
		"vmla.f32 q2, q11, d13[1]\n"
		"vmla.f32 q3, q11, d15[1]\n"

		"vstmia %0, {q0-q3}"
		:
		: "r" (m1.getData()), "r" (m1.getData()), "r" (m2.getData())
		: "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q11"
	);
}

void Math::preMulMatrix4x4NEON(Matrix4x4 &m2,const Matrix4x4 &m1){
	asm volatile(
		"vldmia %1, {q4-q7}\n"
		"vldmia %2, {q8-q11}\n"

		"vmul.f32 q0, q8, d8[0]\n"
		"vmul.f32 q1, q8, d10[0]\n"
		"vmul.f32 q2, q8, d12[0]\n"
		"vmul.f32 q3, q8, d14[0]\n"

		"vmla.f32 q0, q9, d8[1]\n"
		"vmla.f32 q1, q9, d10[1]\n"
		"vmla.f32 q2, q9, d12[1]\n"
		"vmla.f32 q3, q9, d14[1]\n"

		"vmla.f32 q0, q10, d9[0]\n"
		"vmla.f32 q1, q10, d11[0]\n"
		"vmla.f32 q2, q10, d13[0]\n"
		"vmla.f32 q3, q10, d15[0]\n"

		"vmla.f32 q0, q11, d9[1]\n"
		"vmla.f32 q1, q11, d11[1]\n"
		"vmla.f32 q2, q11, d13[1]\n"
		"vmla.f32 q3, q11, d15[1]\n"

		"vstmia %0, {q0-q3}"
		:
		: "r" (m2.getData()), "r" (m1.getData()), "r" (m2.getData())
		: "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q11"
	);
}

void Math::mulVector4Matrix4x4Vector4NEON(Vector4 &r,const Matrix4x4 &m,const Vector4 &v){
	asm volatile(
		"vldmia %1, {q1-q4}\n"
		"vldmia %2, {q5}\n"

		"vmul.f32 q0, q1, d10[0]\n"

		"vmla.f32 q0, q2, d10[1]\n"

		"vmla.f32 q0, q3, d11[0]\n"

		"vmla.f32 q0, q4, d11[1]\n"

		"vstmia %0, {q0}"
		:
		: "r" (r.getData()), "r" (m.getData()), "r" (v.getData())
		: "memory", "q0", "q1", "q2", "q3", "q4", "q5"
	);
}

void Math::mulVector4Matrix4x4NEON(Vector4 &r,const Matrix4x4 &m){
	asm volatile(
		"vldmia %1, {q1-q4}\n"
		"vldmia %2, {q5}\n"

		"vmul.f32 q0, q1, d10[0]\n"

		"vmla.f32 q0, q2, d10[1]\n"

		"vmla.f32 q0, q3, d11[0]\n"

		"vmla.f32 q0, q4, d11[1]\n"

		"vstmia %0, {q0}"
		:
		: "r" (r.getData()), "r" (m.getData()), "r" (r.getData())
		: "memory", "q0", "q1", "q2", "q3", "q4", "q5"
	);
}

#endif

class MathInitializer{
public:
	MathInitializer(){
		Math::init();
	}

	virtual void reference(){}
};

static MathInitializer mathInitializer;

void Math::init(){
	mathInitializer.reference();

	mulMatrix4x4=mulMatrix4x4Traditional;
	preMulMatrix4x4=preMulMatrix4x4Traditional;
	postMulMatrix4x4=postMulMatrix4x4Traditional;
	mulVector4Matrix4x4Vector4=mulVector4Matrix4x4Vector4Traditional;
	mulVector4Matrix4x4=mulVector4Matrix4x4Traditional;
}

void Math::optimize(int o){
	SystemCaps caps;
	System::getSystemCaps(caps);

	Log::alert(Categories::TOADLET_EGG,
		String("Detected SSE version:")+caps.sseVersion);
	Log::alert(Categories::TOADLET_EGG,
		String("Detected NEON version:")+caps.neonVersion);

	#if defined(TOADLET_HAS_SSE)
		if(o==Optimize_AUTO && caps.sseVersion>0){
			// Time different paths to choose most optimal
			static int count=1000;
			Matrix4x4 m,r;
			Vector4 v;
			int i;

			uint64 t0=System::utime();

			r.set(Math::IDENTITY_MATRIX4X4);
			m.set(Math::IDENTITY_MATRIX4X4);
			for(i=0;i<count;++i){
				Math::preMulMatrix4x4Traditional(r,m);
			}

			uint64 t1=System::utime();

			r.set(Math::IDENTITY_MATRIX4X4);
			m.set(Math::IDENTITY_MATRIX4X4);
			for(i=0;i<count;++i){
				Math::preMulMatrix4x4SSE(r,m);
			}

			uint64 t2=System::utime();

			m.set(Math::IDENTITY_MATRIX4X4);
			v.set(Math::X_UNIT_VECTOR4);
			for(i=0;i<count;++i){
				Math::mulVector4Matrix4x4Traditional(v,m);
			}

			uint64 t3=System::utime();

			m.set(Math::IDENTITY_MATRIX4X4);
			v.set(Math::X_UNIT_VECTOR4);
			for(i=0;i<count;++i){
				Math::mulVector4Matrix4x4SSE(v,m);
			}

			uint64 t4=System::utime();

			Log::alert(Categories::TOADLET_EGG,
				String("Timings - MatrixTraditional:")+(t1-t0)+" MatrixSSE:"+(t2-t1)+" VectorTraditional:"+(t3-t2)+" VectorSSE:"+(t4-t3));

			if(t1-t0 < t2-t1){
				Log::excess(Categories::TOADLET_EGG,"using Traditional Matrix4x4 math");

				mulMatrix4x4=mulMatrix4x4Traditional;
				preMulMatrix4x4=preMulMatrix4x4Traditional;
				postMulMatrix4x4=postMulMatrix4x4Traditional;
			}
			else{
				Log::excess(Categories::TOADLET_EGG,"using SSE Matrix4x4 math");

				mulMatrix4x4=mulMatrix4x4SSE;
				preMulMatrix4x4=preMulMatrix4x4SSE;
				postMulMatrix4x4=postMulMatrix4x4SSE;
			}

			if(t3-t2 < t4-t3){
				Log::excess(Categories::TOADLET_EGG,"using Traditional Vector4 math");

				mulVector4Matrix4x4Vector4=mulVector4Matrix4x4Vector4Traditional;
				mulVector4Matrix4x4=mulVector4Matrix4x4Traditional;
			}
			else{
				Log::excess(Categories::TOADLET_EGG,"using SSE Vector4 math");

				mulVector4Matrix4x4Vector4=mulVector4Matrix4x4Vector4SSE;
				mulVector4Matrix4x4=mulVector4Matrix4x4SSE;
			}
		}
		else if(o==Optimize_FORCE && caps.sseVersion>0){
			Log::excess(Categories::TOADLET_EGG,"forcing SSE math");

			mulMatrix4x4=mulMatrix4x4SSE;
			preMulMatrix4x4=preMulMatrix4x4SSE;
			postMulMatrix4x4=postMulMatrix4x4SSE;
			mulVector4Matrix4x4Vector4=mulVector4Matrix4x4Vector4SSE;
			mulVector4Matrix4x4=mulVector4Matrix4x4SSE;
		}
		else
	#elif defined(TOADLET_HAS_NEON)
		if(o==Optimize_AUTO && caps.neonVersion>0){
			// Time different paths to choose most optimal
			static int count=1000;
			Matrix4x4 m,r;
			Vector4 v;
			int i;

			uint64 t0=System::utime();

			r.set(Math::IDENTITY_MATRIX4X4);
			m.set(Math::IDENTITY_MATRIX4X4);
			for(i=0;i<count;++i){
				Math::preMulMatrix4x4Traditional(r,m);
			}

			uint64 t1=System::utime();

			r.set(Math::IDENTITY_MATRIX4X4);
			m.set(Math::IDENTITY_MATRIX4X4);
			for(i=0;i<count;++i){
				Math::preMulMatrix4x4NEON(r,m);
			}

			uint64 t2=System::utime();

			m.set(Math::IDENTITY_MATRIX4X4);
			v.set(Math::X_UNIT_VECTOR4);
			for(i=0;i<count;++i){
				Math::mulVector4Matrix4x4Traditional(v,m);
			}

			uint64 t3=System::utime();

			m.set(Math::IDENTITY_MATRIX4X4);
			v.set(Math::X_UNIT_VECTOR4);
			for(i=0;i<count;++i){
				Math::mulVector4Matrix4x4NEON(v,m);
			}

			uint64 t4=System::utime();

			Log::alert(Categories::TOADLET_EGG,
				String("Timings - MatrixTraditional:")+(t1-t0)+" MatrixNEON:"+(t2-t1)+" VectorTraditional:"+(t3-t2)+" VectorNEON:"+(t4-t3));

			if(t1-t0 < t2-t1){
				Log::excess(Categories::TOADLET_EGG,"using Traditional Matrix4x4 math");

				mulMatrix4x4=mulMatrix4x4Traditional;
				preMulMatrix4x4=preMulMatrix4x4Traditional;
				postMulMatrix4x4=postMulMatrix4x4Traditional;
			}
			else{
				Log::excess(Categories::TOADLET_EGG,"using NEON Matrix4x4 math");

				mulMatrix4x4=mulMatrix4x4NEON;
				preMulMatrix4x4=preMulMatrix4x4NEON;
				postMulMatrix4x4=postMulMatrix4x4NEON;
			}

			if(t3-t2 < t4-t3){
				Log::excess(Categories::TOADLET_EGG,"using Traditional Vector4 math");

				mulVector4Matrix4x4Vector4=mulVector4Matrix4x4Vector4Traditional;
				mulVector4Matrix4x4=mulVector4Matrix4x4Traditional;
			}
			else{
				Log::excess(Categories::TOADLET_EGG,"using NEON Vector4 math");

				mulVector4Matrix4x4Vector4=mulVector4Matrix4x4Vector4NEON;
				mulVector4Matrix4x4=mulVector4Matrix4x4NEON;
			}
		}
		else if(o==Optimize_FORCE && caps.neonVersion>0){
			Log::excess(Categories::TOADLET_EGG,"forcing NEON math");

			mulMatrix4x4=mulMatrix4x4NEON;
			preMulMatrix4x4=preMulMatrix4x4NEON;
			postMulMatrix4x4=postMulMatrix4x4NEON;
			mulVector4Matrix4x4Vector4=mulVector4Matrix4x4Vector4NEON;
			mulVector4Matrix4x4=mulVector4Matrix4x4NEON;
		}
	#endif
	{
		Log::excess(Categories::TOADLET_EGG,"forcing Traditional math");

		init();
	}
}

}
}
}
