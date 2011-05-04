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

#ifndef TOADLET_EGG_MATH_MATRIX4X4_H
#define TOADLET_EGG_MATH_MATRIX4X4_H

#include <toadlet/egg/math/Vector4.h>

namespace toadlet{
namespace egg{
namespace math{

class TOADLET_API TOADLET_ALIGNED Matrix4x4{
public:
	class RowAccessor{
	public:
		inline RowAccessor(Matrix4x4 *m,int r){
			matrix=m;
			row=r;
		}

		inline real &operator[](int col){
			return matrix->at(row,col);
		}

		inline real operator[](int col) const{
			return matrix->at(row,col);
		}

		Matrix4x4 *matrix;
		int row;
	};

	real data[16];

	inline Matrix4x4(){
		data[0]=1;data[4]=0;data[8]=0;data[12]=0;
		data[1]=0;data[5]=1;data[9]=0;data[13]=0;
		data[2]=0;data[6]=0;data[10]=1;data[14]=0;
		data[3]=0;data[7]=0;data[11]=0;data[15]=1;
	}

	inline Matrix4x4(real x1,real x2,real x3,real x4,real y1,real y2,real y3,real y4,real z1,real z2,real z3,real z4,real w1,real w2,real w3,real w4){
		data[0]=x1;data[4]=x2;data[8]=x3;data[12]=x4;
		data[1]=y1;data[5]=y2;data[9]=y3;data[13]=y4;
		data[2]=z1;data[6]=z2;data[10]=z3;data[14]=z4;
		data[3]=w1;data[7]=w2;data[11]=w3;data[15]=w4;
	}

	inline Matrix4x4(float d[]){
		data[0]=d[0];data[4]=d[4];data[8]=d[8];data[12]=d[12];
		data[1]=d[1];data[5]=d[5];data[9]=d[9];data[13]=d[13];
		data[2]=d[2];data[6]=d[6];data[10]=d[10];data[14]=d[14];
		data[3]=d[3];data[7]=d[7];data[11]=d[11];data[15]=d[15];
	}

	inline Matrix4x4(double d[]){
		data[0]=d[0];data[4]=d[4];data[8]=d[8];data[12]=d[12];
		data[1]=d[1];data[5]=d[5];data[9]=d[9];data[13]=d[13];
		data[2]=d[2];data[6]=d[6];data[10]=d[10];data[14]=d[14];
		data[3]=d[3];data[7]=d[7];data[11]=d[11];data[15]=d[15];
	}

	inline Matrix4x4 &set(const Matrix4x4 &matrix){
		*this=matrix;

		return *this;
	}

	inline Matrix4x4 &set(real x1,real x2,real x3,real x4,real y1,real y2,real y3,real y4,real z1,real z2,real z3,real z4,real w1,real w2,real w3,real w4){
		data[0]=x1;data[4]=x2;data[8]=x3;data[12]=x4;
		data[1]=y1;data[5]=y2;data[9]=y3;data[13]=y4;
		data[2]=z1;data[6]=z2;data[10]=z3;data[14]=z4;
		data[3]=w1;data[7]=w2;data[11]=w3;data[15]=w4;

		return *this;
	}

	inline Matrix4x4 &set(float d[]){
		data[0]=d[0];data[4]=d[4];data[8]=d[8];data[12]=d[12];
		data[1]=d[1];data[5]=d[5];data[9]=d[9];data[13]=d[13];
		data[2]=d[2];data[6]=d[6];data[10]=d[10];data[14]=d[14];
		data[3]=d[3];data[7]=d[7];data[11]=d[11];data[15]=d[15];

		return *this;
	}

	inline Matrix4x4 &set(double d[]){
		data[0]=d[0];data[4]=d[4];data[8]=d[8];data[12]=d[12];
		data[1]=d[1];data[5]=d[5];data[9]=d[9];data[13]=d[13];
		data[2]=d[2];data[6]=d[6];data[10]=d[10];data[14]=d[14];
		data[3]=d[3];data[7]=d[7];data[11]=d[11];data[15]=d[15];

		return *this;
	}

	inline Matrix4x4 &reset(){
		data[0]=1;data[4]=0;data[8]=0;data[12]=0;
		data[1]=0;data[5]=1;data[9]=0;data[13]=0;
		data[2]=0;data[6]=0;data[10]=1;data[14]=0;
		data[3]=0;data[7]=0;data[11]=0;data[15]=1;

		return *this;
	}

	inline RowAccessor operator[](int row){
		return RowAccessor(this,row);
	}

	inline const RowAccessor operator[](int row) const{
		return RowAccessor(const_cast<Matrix4x4*>(this),row);
	}

	inline real at(int row,int col) const{
		return data[row + col*4];
	}

	inline real &at(int row,int col){
		return data[row + col*4];
	}

	inline void setAt(int row,int col,real v){
		data[row + col*4]=v;
	}

	inline real *getData(){return data;}
	inline const real *getData() const{return data;}

	inline bool equals(const Matrix4x4 &m2) const{
		return
			data[0]==m2.data[0] && data[1]==m2.data[1] && data[2]==m2.data[2] && data[3]==m2.data[3] &&
			data[4]==m2.data[4] && data[5]==m2.data[5] && data[6]==m2.data[6] && data[7]==m2.data[7] &&
			data[8]==m2.data[8] && data[9]==m2.data[9] && data[10]==m2.data[10] && data[11]==m2.data[11] &&
			data[12]==m2.data[12] && data[13]==m2.data[13] && data[14]==m2.data[14] && data[15]==m2.data[15];
	}

	inline bool operator==(const Matrix4x4 &m2) const{
		return equals(m2);
	}

	inline bool operator!=(const Matrix4x4 &m2) const{
		return !equals(m2);
	}

	Matrix4x4 operator*(const Matrix4x4 &m2) const;

	void operator*=(const Matrix4x4 &m2);

	Vector4 operator*(const Vector4 &v) const;
};

}
}
}

#endif

