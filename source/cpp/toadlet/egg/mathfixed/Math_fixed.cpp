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

#include <toadlet/egg/mathfixed/Math.h>
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/System.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

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
	r.data[0+0*3]=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+0*3]);
	r.data[0+1*3]=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+1*3]);
	r.data[0+2*3]=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+2*3]);

	r.data[1+0*3]=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+0*3]);
	r.data[1+1*3]=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+1*3]);
	r.data[1+2*3]=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+2*3]);

	r.data[2+0*3]=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+0*3]);
	r.data[2+1*3]=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+1*3]);
	r.data[2+2*3]=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+2*3]);
}

void Math::postMul(Matrix3x3 &m1,const Matrix3x3 &m2){
	fixed d00=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+0*3]);
	fixed d01=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+1*3]);
	fixed d02=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+2*3]);

	fixed d10=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+0*3]);
	fixed d11=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+1*3]);
	fixed d12=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+2*3]);

	fixed d20=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+0*3]);
	fixed d21=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+1*3]);
	fixed d22=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+2*3]);

	m1.data[0+0*3]=d00; m1.data[0+1*3]=d01; m1.data[0+2*3]=d02;
	m1.data[1+0*3]=d10; m1.data[1+1*3]=d11; m1.data[1+2*3]=d12;
	m1.data[2+0*3]=d20; m1.data[2+1*3]=d21; m1.data[2+2*3]=d22;
}

void Math::preMul(Matrix3x3 &m2,const Matrix3x3 &m1){
	fixed d00=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+0*3]);
	fixed d01=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+1*3]);
	fixed d02=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+2*3]);

	fixed d10=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+0*3]);
	fixed d11=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+1*3]);
	fixed d12=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+2*3]);

	fixed d20=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+0*3]);
	fixed d21=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+1*3]);
	fixed d22=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+2*3]);

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

void Math::mul(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
	r.data[0+0*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+0*4]);
	r.data[0+1*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+1*4]);
	r.data[0+2*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+2*4]);
	r.data[0+3*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+3*4]);

	r.data[1+0*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+0*4]);
	r.data[1+1*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+1*4]);
	r.data[1+2*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+2*4]);
	r.data[1+3*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+3*4]);

	r.data[2+0*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+0*4]);
	r.data[2+1*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+1*4]);
	r.data[2+2*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+2*4]);
	r.data[2+3*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+3*4]);

	r.data[3+0*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+0*4]);
	r.data[3+1*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+1*4]);
	r.data[3+2*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+2*4]);
	r.data[3+3*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+3*4]);
}

void Math::postMul(Matrix4x4 &m1,const Matrix4x4 &m2){
	fixed d00=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+0*4]);
	fixed d01=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+1*4]);
	fixed d02=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+2*4]);
	fixed d03=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+3*4]);

	fixed d10=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+0*4]);
	fixed d11=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+1*4]);
	fixed d12=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+2*4]);
	fixed d13=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+3*4]);

	fixed d20=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+0*4]);
	fixed d21=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+1*4]);
	fixed d22=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+2*4]);
	fixed d23=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+3*4]);

	fixed d30=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+0*4]);
	fixed d31=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+1*4]);
	fixed d32=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+2*4]);
	fixed d33=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+3*4]);

	m1.data[0+0*4]=d00; m1.data[0+1*4]=d01; m1.data[0+2*4]=d02; m1.data[0+3*4]=d03;
	m1.data[1+0*4]=d10; m1.data[1+1*4]=d11; m1.data[1+2*4]=d12; m1.data[1+3*4]=d13;
	m1.data[2+0*4]=d20; m1.data[2+1*4]=d21; m1.data[2+2*4]=d22; m1.data[2+3*4]=d23;
	m1.data[3+0*4]=d30; m1.data[3+1*4]=d31; m1.data[3+2*4]=d32; m1.data[3+3*4]=d33;
}

void Math::preMul(Matrix4x4 &m2,const Matrix4x4 &m1){
	fixed d00=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+0*4]);
	fixed d01=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+1*4]);
	fixed d02=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+2*4]);
	fixed d03=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+3*4]);

	fixed d10=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+0*4]);
	fixed d11=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+1*4]);
	fixed d12=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+2*4]);
	fixed d13=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+3*4]);

	fixed d20=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+0*4]);
	fixed d21=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+1*4]);
	fixed d22=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+2*4]);
	fixed d23=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+3*4]);

	fixed d30=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+0*4]);
	fixed d31=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+1*4]);
	fixed d32=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+2*4]);
	fixed d33=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+3*4]);

	m2.data[0+0*4]=d00; m2.data[0+1*4]=d01; m2.data[0+2*4]=d02; m2.data[0+3*4]=d03;
	m2.data[1+0*4]=d10; m2.data[1+1*4]=d11; m2.data[1+2*4]=d12; m2.data[1+3*4]=d13;
	m2.data[2+0*4]=d20; m2.data[2+1*4]=d21; m2.data[2+2*4]=d22; m2.data[2+3*4]=d23;
	m2.data[3+0*4]=d30; m2.data[3+1*4]=d31; m2.data[3+2*4]=d32; m2.data[3+3*4]=d33;
}

void Math::transpose(Matrix3x3 &r,const Matrix3x3 &m){
	r.data[0+0*3]=m.data[0+0*3]; r.data[0+1*3]=m.data[1+0*3]; r.data[0+2*3]=m.data[2+0*3];
	r.data[1+0*3]=m.data[0+1*3]; r.data[1+1*3]=m.data[1+1*3]; r.data[1+2*3]=m.data[2+1*3];
	r.data[2+0*3]=m.data[0+2*3]; r.data[2+1*3]=m.data[1+2*3]; r.data[2+2*3]=m.data[2+2*3];
}

fixed Math::determinant(const Matrix3x3 &m){
	return	-TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+0*3],m.data[1+1*3]),m.data[2+2*3]) + TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+0*3],m.data[2+1*3]),m.data[1+2*3]) +
			 TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+1*3],m.data[1+0*3]),m.data[2+2*3]) - TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+1*3],m.data[2+0*3]),m.data[1+2*3]) -
			 TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+2*3],m.data[1+0*3]),m.data[2+1*3]) + TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+2*3],m.data[2+0*3]),m.data[1+1*3]);
}

bool Math::invert(Matrix3x3 &r,const Matrix3x3 &m){
	fixed det=TOADLET_DIV_XX(ONE,determinant(m));
	if(det==0){
		return false;
	}
	else{
		r.data[0+0*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+1*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+1*3],m.data[1+2*3]));
		r.data[1+0*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+0*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[1+2*3]));
		r.data[2+0*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+0*3],m.data[2+1*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[1+1*3]));

		r.data[0+1*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+1*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+1*3],m.data[0+2*3]));
		r.data[1+1*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[0+2*3]));
		r.data[2+1*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[2+1*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[0+1*3]));

		r.data[0+2*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+1*3],m.data[1+2*3]) - TOADLET_MUL_XX(m.data[1+1*3],m.data[0+2*3]));
		r.data[1+2*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[1+2*3]) - TOADLET_MUL_XX(m.data[1+0*3],m.data[0+2*3]));
		r.data[2+2*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[1+1*3]) - TOADLET_MUL_XX(m.data[1+0*3],m.data[0+1*3]));

		return true;
	}
}

void Math::transpose(Matrix4x4 &r,const Matrix4x4 &m){
	r.data[0+0*4]=m.data[0+0*4]; r.data[0+1*4]=m.data[1+0*4]; r.data[0+2*4]=m.data[2+0*4]; r.data[0+3*4]=m.data[3+0*4];
	r.data[1+0*4]=m.data[0+1*4]; r.data[1+1*4]=m.data[1+1*4]; r.data[1+2*4]=m.data[2+1*4]; r.data[1+3*4]=m.data[3+1*4];
	r.data[2+0*4]=m.data[0+2*4]; r.data[2+1*4]=m.data[1+2*4]; r.data[2+2*4]=m.data[2+2*4]; r.data[2+3*4]=m.data[3+2*4];
	r.data[3+0*4]=m.data[0+3*4]; r.data[3+1*4]=m.data[1+3*4]; r.data[3+2*4]=m.data[2+3*4]; r.data[3+3*4]=m.data[3+3*4];
}

fixed Math::determinant(const Matrix4x4 &m){
	fixed det1 = TOADLET_MUL_XX(m.data[1+2*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[1+3*4]);
	fixed det2 = TOADLET_MUL_XX(m.data[1+1*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[1+3*4]);
	fixed det3 = TOADLET_MUL_XX(m.data[1+1*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[1+2*4]);
	fixed det4 = TOADLET_MUL_XX(m.data[1+0*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[1+3*4]);
	fixed det5 = TOADLET_MUL_XX(m.data[1+0*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[1+2*4]);
	fixed det6 = TOADLET_MUL_XX(m.data[1+0*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[1+1*4]);

	return -TOADLET_MUL_XX(m.data[3+0*4],TOADLET_MUL_XX(m.data[0+1*4],det1) - TOADLET_MUL_XX(m.data[0+2*4],det2) + TOADLET_MUL_XX(m.data[0+3*4],det3)) +
			TOADLET_MUL_XX(m.data[3+1*4],TOADLET_MUL_XX(m.data[0+0*4],det1) - TOADLET_MUL_XX(m.data[0+2*4],det4) + TOADLET_MUL_XX(m.data[0+3*4],det5)) -
			TOADLET_MUL_XX(m.data[3+2*4],TOADLET_MUL_XX(m.data[0+0*4],det2) - TOADLET_MUL_XX(m.data[0+1*4],det4) + TOADLET_MUL_XX(m.data[0+3*4],det6)) +
			TOADLET_MUL_XX(m.data[3+3*4],TOADLET_MUL_XX(m.data[0+0*4],det3) - TOADLET_MUL_XX(m.data[0+1*4],det5) + TOADLET_MUL_XX(m.data[0+2*4],det6));
}

bool Math::invert(Matrix4x4 &r,const Matrix4x4 &m){
	fixed det=determinant(m);

	if(abs(det)==0){
		return false;
	}
	else{
		det=TOADLET_DIV_XX(ONE,det);
		r.data[0+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+1*4],(TOADLET_MUL_XX(m.data[2+2*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[2+3*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[1+2*4],(TOADLET_MUL_XX(m.data[2+3*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[3+3*4]))) + TOADLET_MUL_XX(m.data[1+3*4],(TOADLET_MUL_XX(m.data[2+1*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[3+1*4]))));
		r.data[0+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+1*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[2+2*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[3+3*4]))) + TOADLET_MUL_XX(m.data[2+3*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[3+1*4]))));
		r.data[0+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+1*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[1+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[1+2*4]))) + TOADLET_MUL_XX(m.data[3+2*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[1+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[1+3*4]))) + TOADLET_MUL_XX(m.data[3+3*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[1+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[1+1*4]))));
		r.data[0+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+1*4],(TOADLET_MUL_XX(m.data[1+3*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[1+2*4],m.data[2+3*4]))) + TOADLET_MUL_XX(m.data[0+2*4],(TOADLET_MUL_XX(m.data[1+1*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[1+3*4],m.data[2+1*4]))) + TOADLET_MUL_XX(m.data[0+3*4],(TOADLET_MUL_XX(m.data[1+2*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[1+1*4],m.data[2+2*4]))));

		r.data[1+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+2*4],(TOADLET_MUL_XX(m.data[2+0*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[2+3*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[1+3*4],(TOADLET_MUL_XX(m.data[2+2*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[1+0*4],(TOADLET_MUL_XX(m.data[2+3*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[3+3*4]))));
		r.data[1+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+2*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[2+3*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[2+0*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[3+3*4]))));
		r.data[1+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+2*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[1+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[1+0*4]))) + TOADLET_MUL_XX(m.data[3+3*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[1+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[1+2*4]))) + TOADLET_MUL_XX(m.data[3+0*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[1+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[1+3*4]))));
		r.data[1+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+2*4],(TOADLET_MUL_XX(m.data[1+3*4],m.data[2+0*4]) - TOADLET_MUL_XX(m.data[1+0*4],m.data[2+3*4]))) + TOADLET_MUL_XX(m.data[0+3*4],(TOADLET_MUL_XX(m.data[1+0*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[1+2*4],m.data[2+0*4]))) + TOADLET_MUL_XX(m.data[0+0*4],(TOADLET_MUL_XX(m.data[1+2*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[1+3*4],m.data[2+2*4]))));

		r.data[2+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+3*4],(TOADLET_MUL_XX(m.data[2+0*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[1+0*4],(TOADLET_MUL_XX(m.data[2+1*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[2+3*4],m.data[3+1*4]))) + TOADLET_MUL_XX(m.data[1+1*4],(TOADLET_MUL_XX(m.data[2+3*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[3+3*4]))));
		r.data[2+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+3*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[2+0*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[3+1*4]))) + TOADLET_MUL_XX(m.data[2+1*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[3+3*4]))));
		r.data[2+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+3*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[1+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[1+0*4]))) + TOADLET_MUL_XX(m.data[3+0*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[1+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[1+1*4]))) + TOADLET_MUL_XX(m.data[3+1*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[1+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[1+3*4]))));
		r.data[2+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+3*4],(TOADLET_MUL_XX(m.data[1+1*4],m.data[2+0*4]) - TOADLET_MUL_XX(m.data[1+0*4],m.data[2+1*4]))) + TOADLET_MUL_XX(m.data[0+0*4],(TOADLET_MUL_XX(m.data[1+3*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[1+1*4],m.data[2+3*4]))) + TOADLET_MUL_XX(m.data[0+1*4],(TOADLET_MUL_XX(m.data[1+0*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[1+3*4],m.data[2+0*4]))));

		r.data[3+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+0*4],(TOADLET_MUL_XX(m.data[2+2*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[1+1*4],(TOADLET_MUL_XX(m.data[2+0*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[1+2*4],(TOADLET_MUL_XX(m.data[2+1*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[3+1*4]))));
		r.data[3+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+0*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[2+1*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[2+2*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[3+1*4]))));
		r.data[3+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+0*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[1+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[1+2*4]))) + TOADLET_MUL_XX(m.data[3+1*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[1+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[1+0*4]))) + TOADLET_MUL_XX(m.data[3+2*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[1+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[1+1*4]))));
		r.data[3+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*4],(TOADLET_MUL_XX(m.data[1+1*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[1+2*4],m.data[2+1*4]))) + TOADLET_MUL_XX(m.data[0+1*4],(TOADLET_MUL_XX(m.data[1+2*4],m.data[2+0*4]) - TOADLET_MUL_XX(m.data[1+0*4],m.data[2+2*4]))) + TOADLET_MUL_XX(m.data[0+2*4],(TOADLET_MUL_XX(m.data[1+0*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[1+1*4],m.data[2+0*4]))));

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

void Math::setMatrix4x4FromOrtho(Matrix4x4 &r,fixed leftDist,fixed rightDist,fixed bottomDist,fixed topDist,fixed nearDist,fixed farDist){
	r.set(
		div(TWO,rightDist-leftDist),0,0,-div(rightDist+leftDist,rightDist-leftDist),
		0,div(TWO,topDist-bottomDist),0,-div(topDist+bottomDist,topDist-bottomDist),
		0,0,-div(TWO,farDist-nearDist),-div(farDist+nearDist,farDist-nearDist),
		0,0,0,ONE
	);
}

void Math::setMatrix4x4FromFrustum(Matrix4x4 &r,fixed leftDist,fixed rightDist,fixed bottomDist,fixed topDist,fixed nearDist,fixed farDist){
	r.set(
		div(nearDist<<1,rightDist-leftDist),0,div(rightDist+leftDist,rightDist-leftDist),0,
		0,div(nearDist<<1,topDist-bottomDist),div(topDist+bottomDist,topDist-bottomDist),0,
		0,0,-div(farDist+nearDist,farDist-nearDist),-div(mul(farDist,nearDist)<<1,farDist-nearDist),
		0,0,-ONE,0
	);
}

void Math::setMatrix4x4FromPerspectiveX(Matrix4x4 &r,fixed fovx,fixed aspect,fixed nearDist,fixed farDist){
	fixed h=tan(fovx>>1);

	r.set(
		div(ONE,h),0,0,0,
		0,div(ONE,mul(h,aspect)),0,0,
		0,0,div(farDist+nearDist,nearDist-farDist),div(mul(farDist,nearDist)<<1,nearDist-farDist),
		0,0,-ONE,0
	);
}

void Math::setMatrix4x4FromPerspectiveY(Matrix4x4 &r,fixed fovy,fixed aspect,fixed nearDist,fixed farDist){
	fixed h=tan(fovy>>1);

	r.set(
		div(ONE,mul(h,aspect)),0,0,0,
		0,div(ONE,h),0,0,
		0,0,div(farDist+nearDist,nearDist-farDist),div(mul(farDist,nearDist)<<1,nearDist-farDist),
		0,0,-ONE,0
	);
}


void Math::setMatrix4x4FromObliquePlane(Matrix4x4 &r,const Plane &plane,const Vector3 &dir){
	fixed nd=Math::dot(plane.normal,dir);

	r.set(
		TOADLET_MUL_XX(dir.x,plane.normal.x) - nd, TOADLET_MUL_XX(dir.x,plane.normal.y), TOADLET_MUL_XX(dir.x,plane.normal.z), -TOADLET_MUL_XX(plane.distance,dir.x),
		TOADLET_MUL_XX(dir.y,plane.normal.x), TOADLET_MUL_XX(dir.y,plane.normal.y) - nd, TOADLET_MUL_XX(dir.y,plane.normal.z), -TOADLET_MUL_XX(plane.distance,dir.y),
		TOADLET_MUL_XX(dir.z,plane.normal.x), TOADLET_MUL_XX(dir.z,plane.normal.y), TOADLET_MUL_XX(dir.z,plane.normal.z) - nd, -TOADLET_MUL_XX(plane.distance,dir.z),
		0,0,0,-nd
	);
}

void Math::setMatrix4x4FromPerspectivePlane(Matrix4x4 &r,const Plane &plane,const Vector3 &eye){
	Vector3 cacheEp;

	cacheEp.set(eye.x-TOADLET_MUL_XX(plane.normal.x,plane.distance),eye.y-TOADLET_MUL_XX(plane.normal.y,plane.distance),eye.z-TOADLET_MUL_XX(plane.normal.z,plane.distance));
	fixed nd=Math::dot(plane.normal,cacheEp);

	r.set(
		nd - TOADLET_MUL_XX(eye.x,plane.normal.x),-TOADLET_MUL_XX(eye.x,plane.normal.y),-TOADLET_MUL_XX(eye.x,plane.normal.z), 0,
		-TOADLET_MUL_XX(eye.y,plane.normal.x),nd - TOADLET_MUL_XX(eye.y,plane.normal.y),-TOADLET_MUL_XX(eye.y,plane.normal.z), 0,
		-TOADLET_MUL_XX(eye.z,plane.normal.x),-TOADLET_MUL_XX(eye.z,plane.normal.y),nd - TOADLET_MUL_XX(eye.z,plane.normal.z), 0,
		-plane.normal.x,-plane.normal.y,-plane.normal.z,Math::dot(eye,plane.normal)
	);

	r.data[12] = -(TOADLET_MUL_XX(r.data[0],eye.x) + TOADLET_MUL_XX(r.data[4],eye.y) + TOADLET_MUL_XX(r.data[8],eye.z));
	r.data[13] = -(TOADLET_MUL_XX(r.data[1],eye.x) + TOADLET_MUL_XX(r.data[5],eye.y) + TOADLET_MUL_XX(r.data[9],eye.z));
	r.data[14] = -(TOADLET_MUL_XX(r.data[2],eye.x) + TOADLET_MUL_XX(r.data[6],eye.y) + TOADLET_MUL_XX(r.data[10],eye.z));
}

fixed Math::setAxisAngleFromQuaternion(Vector3 &axis,const Quaternion &q,fixed epsilon){
	fixed angle=acos(q.w)<<1;

	fixed sinHalfAngle=sin(angle>>1);
	if(sinHalfAngle>epsilon){
		fixed invSinHalfAngle=div(ONE,sinHalfAngle);
		axis.x=mul(q.x,invSinHalfAngle);
		axis.y=mul(q.y,invSinHalfAngle);
		axis.z=mul(q.z,invSinHalfAngle);

		fixed l=sqrt((mul(axis.x,axis.x)+mul(axis.y,axis.y)+mul(axis.z,axis.z)));
		axis.x=div(axis.x,l);
		axis.y=div(axis.y,l);
		axis.z=div(axis.z,l);
	}
	else{
		axis.x=ONE;
		axis.y=0;
		axis.z=0;
	}
	
	return angle;
}

// Algorithm from Ken Shoemake's article in 1987 SIGGRAPH course notes
// article "Quaternion Calculus and Fast Animation".
template<class Matrix>
void setQuaternionFromMatrix(Quaternion &r,const Matrix &m){
	fixed trace=m.at(0,0)+m.at(1,1)+m.at(2,2);
	fixed root;

	if(trace>0){
		// |w| > 1/2, may as well choose w > 1/2
		root=Math::sqrt(trace+Math::ONE); // 2w
		r.w=root>>1;
		root=Math::div(Math::HALF,root); // 1/(4w)
		r.x=Math::mul(m.at(2,1)-m.at(1,2),root);
		r.y=Math::mul(m.at(0,2)-m.at(2,0),root);
		r.z=Math::mul(m.at(1,0)-m.at(0,1),root);
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

		root=Math::sqrt(m.at(i,i)-m.at(j,j)-m.at(k,k)+Math::ONE);
		r[i]=root>>1;
		root=Math::div(Math::HALF,root);
		r.w=Math::mul(m.at(k,j)-m.at(j,k),root);
		r[j]=Math::mul(m.at(j,i)+m.at(i,j),root);
		r[k]=Math::mul(m.at(k,i)+m.at(i,k),root);
	}
}

void Math::setQuaternionFromMatrix3x3(Quaternion &r,const Matrix3x3 &m){
	Vector3 scale;
	setScaleFromMatrix3x3(scale,m);
	Matrix3x3 m2;
	m2.setAt(0,0,TOADLET_DIV_XX(m.at(0,0),scale.x));
	m2.setAt(1,0,TOADLET_DIV_XX(m.at(1,0),scale.x));
	m2.setAt(2,0,TOADLET_DIV_XX(m.at(2,0),scale.x));
	m2.setAt(0,1,TOADLET_DIV_XX(m.at(0,1),scale.y));
	m2.setAt(1,1,TOADLET_DIV_XX(m.at(1,1),scale.y));
	m2.setAt(2,1,TOADLET_DIV_XX(m.at(2,1),scale.y));
	m2.setAt(0,2,TOADLET_DIV_XX(m.at(0,2),scale.z));
	m2.setAt(1,2,TOADLET_DIV_XX(m.at(1,2),scale.z));
	m2.setAt(2,2,TOADLET_DIV_XX(m.at(2,2),scale.z));
	setQuaternionFromMatrix(r,m2);
}

void Math::setQuaternionFromMatrix4x4(Quaternion &r,const Matrix4x4 &m){
	Vector3 scale;
	setScaleFromMatrix4x4(scale,m);
	Matrix4x4 m2(m);
	m2.setAt(0,0,TOADLET_DIV_XX(m.at(0,0),scale.x));
	m2.setAt(1,0,TOADLET_DIV_XX(m.at(1,0),scale.x));
	m2.setAt(2,0,TOADLET_DIV_XX(m.at(2,0),scale.x));
	m2.setAt(0,1,TOADLET_DIV_XX(m.at(0,1),scale.y));
	m2.setAt(1,1,TOADLET_DIV_XX(m.at(1,1),scale.y));
	m2.setAt(2,1,TOADLET_DIV_XX(m.at(2,1),scale.y));
	m2.setAt(0,2,TOADLET_DIV_XX(m.at(0,2),scale.z));
	m2.setAt(1,2,TOADLET_DIV_XX(m.at(1,2),scale.z));
	m2.setAt(2,2,TOADLET_DIV_XX(m.at(2,2),scale.z));
	setQuaternionFromMatrix(r,m2);
}

void Math::lerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,fixed t){
	fixed cosom=dot(q1,q2);

	fixed scl1=(ONE-t);
	fixed scl2=0;
	if(cosom<0){
		scl2=-t;
	}
	else{
		scl2=t;
	}

	r.x=mul(scl1,q1.x) + mul(scl2,q2.x);
	r.y=mul(scl1,q1.y) + mul(scl2,q2.y);
	r.z=mul(scl1,q1.z) + mul(scl2,q2.z);
	r.w=mul(scl1,q1.w) + mul(scl2,q2.w);
}

// The logic behind this slerp function is from the GGT (http://ggt.sourceforge.net)
void Math::slerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,fixed t){
	fixed cosom=dot(q1,q2);

	fixed scl2sign=ONE;
	if(cosom<0){
		cosom=-cosom;
		scl2sign=-ONE;
	}

	fixed scl1=0;
	fixed scl2=0;
	if((ONE-cosom) > 6){
		// Standard slerp
		fixed omega=acos(cosom);
		fixed sinom=sin(omega);
		scl1=div(sin(mul(ONE-t,omega)),sinom);
		scl2=div(sin(mul(t,omega)),sinom);
	}
	else{
		// Very close, just lerp
		scl1=ONE-t;
		scl2=t;
	}
	scl2=mul(scl2,scl2sign);

	r.x=mul(scl1,q1.x) + mul(scl2,q2.x);
	r.y=mul(scl1,q1.y) + mul(scl2,q2.y);
	r.z=mul(scl1,q1.z) + mul(scl2,q2.z);
	r.w=mul(scl1,q1.w) + mul(scl2,q2.w);
}

void Math::project(Vector3 &result,const Segment &segment,const Vector3 &point,bool limitToSegment){
	const Vector3 &o=segment.origin;
	const Vector3 &d=segment.direction;

	if(d.x==0 && d.y==0 && d.z==0){
		result=o;
	}
	else{
		fixed u=div(mul(d.x,point.x-o.x)+mul(d.y,point.y-o.y)+mul(d.z,point.z-o.z),mul(d.x,d.x)+mul(d.y,d.y)+mul(d.z,d.z));
		if(limitToSegment){
			if(u<0) u=0;
			else if(u>ONE) u=ONE;
		}
		result.x=o.x+mul(u,d.x);
		result.y=o.y+mul(u,d.y);
		result.z=o.z+mul(u,d.z);
	}
}

void Math::project(Vector3 &point1,Vector3 &point2,const Segment &seg1, const Segment &seg2,fixed epsilon){
	// For a full discussion of this method see Dan Sunday's Geometry Algorithms web site: http://www.geometryalgorithms.com/
	fixed a=dot(seg1.direction,seg1.direction);
	fixed b=dot(seg1.direction,seg2.direction);
	fixed c=dot(seg2.direction,seg2.direction);

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
	fixed d=dot(seg1.direction,point1);
	fixed e=dot(seg2.direction,point1);
	fixed denom=mul(a,c)-mul(b,b);
	
	fixed u1=0;
	fixed u2=0;
	fixed u1N=0;
	fixed u2N=0;
	fixed u1D=denom;
	fixed u2D=denom;
	
	if(denom<100/* a small fixed point value */){
		// Segments are practically parallel
		u1N=0;
		u1D=ONE;
		u2N=e;
		u2D=c;
	}
	else{
		// Find the closest point on 2 infinite lines
		u1N=mul(b,e)-mul(c,d);
		u2N=mul(a,e)-mul(b,d);
		
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
	
	u1=div(u1N,u1D);
	u2=div(u2N,u2D);

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

	fixed fx=div((fixed)(x-(int)viewportX)<<1,(fixed)viewportWidth)-ONE;
	fixed fy=div((fixed)((int)viewportHeight-(y-(int)viewportY))<<1,(fixed)viewportHeight)-ONE;

	result.origin.set(fx,fy,-ONE);
	mulPoint3Full(result.origin,scratchMatrix);

	result.direction.set(fx,fy,ONE);
	mulPoint3Full(result.direction,scratchMatrix);

	sub(result.direction,result.origin);

	return true;
}

int Math::setClipPlanesFromProjectionMatrix(Plane *planes,int maxNumPlanes,const Matrix4x4 &m){
	const fixed *d=m.data;

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

bool Math::getIntersectionOfThreePlanes(Vector3 &result,const Plane &p1,const Plane &p2,const Plane &p3,fixed epsilon){
	Vector3 p2xp3;
	cross(p2xp3,p2.normal,p3.normal);
	fixed den=dot(p1.normal,p2xp3);
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
	fixed d=dot(plane1.normal,plane2.normal);
	fixed determinant=mul(lengthSquared(plane1.normal),lengthSquared(plane2.normal)) - mul(d,d);

	fixed c1=div(mul(plane1.distance,lengthSquared(plane2.normal)) - mul(plane2.distance,d),determinant);
	fixed c2=div(mul(plane2.distance,lengthSquared(plane1.normal)) - mul(plane1.distance,d),determinant);

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

	fixed f=0;

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

	r.origin.x=(mins.x+maxs.x)>>1;
	r.origin.y=(mins.y+maxs.y)>>1;
	r.origin.z=(mins.z+maxs.z)>>1;

	r.setRadius(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)+mul(maxs.z-r.origin.z,maxs.z-r.origin.z)));
}

void Math::findFitSphere(Sphere &r,const AABox &box){
	const Vector3 &mins=box.mins;
	const Vector3 &maxs=box.maxs;

	r.origin.x=(mins.x+maxs.x)>>1;
	r.origin.y=(mins.y+maxs.y)>>1;
	r.origin.z=(mins.z+maxs.z)>>1;

	r.setRadius(div(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)+mul(maxs.z-r.origin.z,maxs.z-r.origin.z)),intToFixed(3)));
}

void Math::findBoundingCapsule(Capsule &r,const AABox &box){
	const Vector3 &mins=box.mins;
	const Vector3 &maxs=box.maxs;

	r.origin.x=(mins.x+maxs.x)>>1;
	r.origin.y=(mins.y+maxs.y)>>1;
	r.origin.z=mins.z;

	r.direction.z=(maxs.z-mins.z);

	r.setRadius(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)));
}

void Math::findFitCapsule(Capsule &r,const AABox &box){
	const Vector3 &mins=box.mins;
	const Vector3 &maxs=box.maxs;

	r.origin.x=(mins.x+maxs.x)>>1;
	r.origin.y=(mins.y+maxs.y)>>1;

	r.setRadius(div(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)),SQRT_2));

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
	fixed s=0;
	fixed d=0;

	if(sphere.origin.x<box.mins.x){
		s=sphere.origin.x-box.mins.x;
		d+=mul(s,s);
	}
	else if(sphere.origin.x>box.maxs.x){
		s=sphere.origin.x-box.maxs.x;
		d+=mul(s,s);
	}

	if(sphere.origin.y<box.mins.y){
		s=sphere.origin.y-box.mins.y;
		d+=mul(s,s);
	}
	else if(sphere.origin.y>box.maxs.y){
		s=sphere.origin.y-box.maxs.y;
		d+=mul(s,s);
	}

	if(sphere.origin.z<box.mins.z){
		s=sphere.origin.z-box.mins.z;
		d+=mul(s,s);
	}
	else if(sphere.origin.z>box.maxs.z){
		s=sphere.origin.z-box.maxs.z;
		d+=mul(s,s);
	}

	return d<=square(sphere.radius);
}

bool Math::testIntersection(const Sphere &sphere,const Plane *planes,int numPlanes){
	if(sphere.radius<0) return true;
	scalar distance=0;
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

fixed Math::findIntersection(const Segment &segment,const Plane &plane,Vector3 &point,Vector3 &normal){
	fixed d=dot(plane.normal,segment.direction);
	if(d!=0){
		fixed t=div(plane.distance-dot(plane.normal,segment.origin),d);
		mul(point,segment.direction,t);
		add(point,segment.origin);
		normal.set(plane.normal);
		if(t<0 || t>ONE){
			return ONE;
		}
		else{
			return t;
		}
	}
	else{
		return ONE;
	}
}

fixed Math::findIntersection(const Segment &segment,const Sphere &sphere,Vector3 &point,Vector3 &normal){
	const Vector3 &segOrigin=segment.origin;
	const Vector3 &segDirection=segment.direction;
	const Vector3 &sphOrigin=sphere.origin;
	fixed sphRadius=sphere.radius;
	
	// Quadratic Q(t) = a*t^2 + 2*b*t + c
	Vector3 diff;
	sub(diff,segOrigin,sphOrigin);
	fixed a=lengthSquared(segDirection);
	if(a<=0){
		return ONE;
	}
	fixed b=dot(diff,segDirection);
	fixed c=lengthSquared(diff)-square(sphRadius);
	fixed time1=ONE;
	fixed time2=ONE;

	// If no real roots, then no intersection
	fixed discr=square(b) - mul(a,c);
	if(discr<0){
		time1=ONE;
	}
	else if(discr>0){
		fixed root=sqrt(discr);
		fixed invA=div(ONE,a);
		time1=mul((-b)-root,invA);
		time2=mul((-b)+root,invA);

		if(time1>ONE || time2<0){
			time1=ONE;
		}
		else if(time1>=0){
			mul(point,segDirection,time1);
			add(point,segOrigin);
		}
		else{
			mul(point,segDirection,time2);
			add(point,segOrigin);
		}
	}
	else{
		time1=div(-b,a);
		if(0<=time1 && time1<=ONE){
			mul(point,segDirection,time1);
			add(point,segOrigin);
		}
		else{
			time1=ONE;
		}
	}

	if(time1!=ONE){
		sub(normal,point,sphOrigin);
		normalizeCarefully(normal,0);
	}
	return time1;
}

fixed Math::findIntersection(const Segment &segment,const AABox &box,Vector3 &point,Vector3 &normal){
	const Vector3 &segOrigin=segment.origin;
	const Vector3 &segDirection=segment.direction;
	const Vector3 &boxMins=box.mins;
	const Vector3 &boxMaxs=box.maxs;

	bool inside=true;
	char quadX,quadY,quadZ;
	int whichPlane;
	fixed maxTX=0,maxTY=0,maxTZ=0;
	fixed candPlaneX=0,candPlaneY=0,candPlaneZ=0;
	fixed candNormX=-ONE;
	fixed candNormY=-ONE;
	fixed candNormZ=-ONE;
	fixed time=ONE;

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
		candNormX=ONE;
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
		candNormY=ONE;
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
		candNormZ=ONE;
		inside=false;
	}
	else{
		quadZ=2; // Middle
	}

	// Inside
	if(inside){
		return 0;
	}

	// Calculate t distances to candidate planes
	// X
	if(quadX!=2 /*Middle*/ && segDirection.x!=0){
		maxTX=TOADLET_DIV_XX(candPlaneX-segOrigin.x,segDirection.x);
	}
	else{
		maxTX=-ONE;
	}
	// Y
	if(quadY!=2 /*Middle*/ && segDirection.y!=0){
		maxTY=TOADLET_DIV_XX(candPlaneY-segOrigin.y,segDirection.y);
	}
	else{
		maxTY=-ONE;
	}
	// Z
	if(quadZ!=2 /*Middle*/ && segDirection.z!=0){
		maxTZ=TOADLET_DIV_XX(candPlaneZ-segOrigin.z,segDirection.z);
	}
	else{
		maxTZ=-ONE;
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
	if(time<0 || time>ONE){
		return ONE;
	}
	// X
	if(whichPlane!=0){
		point.x=segOrigin.x+TOADLET_MUL_XX(time,segDirection.x);
		if(point.x<boxMins.x || point.x>boxMaxs.x){
			return ONE;
		}
	}
	else{
		point.x=candPlaneX;
	}
	// Y
	if(whichPlane!=1){
		point.y=segOrigin.y+TOADLET_MUL_XX(time,segDirection.y);
		if(point.y<boxMins.y || point.y>boxMaxs.y){
			return ONE;
		}
	}
	else{
		point.y=candPlaneY;
	}
	// Z
	if(whichPlane!=2){
		point.z=segOrigin.z+TOADLET_MUL_XX(time,segDirection.z);
		if(point.z<boxMins.z || point.z>boxMaxs.z){
			return ONE;
		}
	}
	else{
		point.z=candPlaneZ;
	}

	return time;
}

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

	// Assign default implementations
}

void Math::optimize(int o){
	SystemCaps caps;
	System::getSystemCaps(caps);

	Log::alert(Categories::TOADLET_EGG,String("Detected SSE version:")+caps.sseVersion);
	Log::alert(Categories::TOADLET_EGG,String("Detected NEON version:")+caps.neonVersion);
	{
		Log::excess(Categories::TOADLET_EGG,"forcing Traditional math");

		init();
	}
}

}
}
}
