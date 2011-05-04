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

#ifndef TOADLET_EGG_MATHFIXED_MATRIX3X3_H
#define TOADLET_EGG_MATHFIXED_MATRIX3X3_H

#include <toadlet/egg/mathfixed/Vector3.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class TOADLET_API TOADLET_ALIGNED Matrix3x3{
public:
	class RowAccessor{
	public:
		inline RowAccessor(Matrix3x3 *m,int r){
			matrix=m;
			row=r;
		}

		inline fixed &operator[](int col){
			return matrix->at(row,col);
		}

		inline fixed operator[](int col) const{
			return matrix->at(row,col);
		}

		Matrix3x3 *matrix;
		int row;
	};

	fixed data[9];

	inline Matrix3x3(){
		data[0]=Math::ONE;data[3]=0;data[6]=0;
		data[1]=0;data[4]=Math::ONE;data[7]=0;
		data[2]=0;data[5]=0;data[8]=Math::ONE;
	}

	inline Matrix3x3(fixed x1,fixed x2,fixed x3,fixed y1,fixed y2,fixed y3,fixed z1,fixed z2,fixed z3){
		data[0]=x1;data[3]=x2;data[6]=x3;
		data[1]=y1;data[4]=y2;data[7]=y3;
		data[2]=z1;data[5]=z2;data[8]=z3;
	}

	inline Matrix3x3(fixed d[]){
		*this=*(Matrix3x3*)d;
	}

	inline Matrix3x3 &set(const Matrix3x3 &matrix){
		*this=matrix;

		return *this;
	}

	inline Matrix3x3 &set(fixed x1,fixed x2,fixed x3,fixed y1,fixed y2,fixed y3,fixed z1,fixed z2,fixed z3){
		data[0]=x1;data[3]=x2;data[6]=x3;
		data[1]=y1;data[4]=y2;data[7]=y3;
		data[2]=z1;data[5]=z2;data[8]=z3;

		return *this;
	}

	inline Matrix3x3 &set(fixed d[]){
		*this=*(Matrix3x3*)d;

		return *this;
	}

	inline Matrix3x3 &reset(){
		data[0]=Math::ONE;data[3]=0;data[6]=0;
		data[1]=0;data[4]=Math::ONE;data[7]=0;
		data[2]=0;data[5]=0;data[8]=Math::ONE;

		return *this;
	}

	inline RowAccessor operator[](int row){
		return RowAccessor(this,row);
	}

	inline const RowAccessor operator[](int row) const{
		return RowAccessor(const_cast<Matrix3x3*>(this),row);
	}

	inline fixed at(int row,int col) const{
		return data[row + col*3];
	}

	inline fixed &at(int row,int col){
		return data[row + col*3];
	}

	inline void setAt(int row,int col,fixed v){
		data[row + col*3]=v;
	}

	inline fixed *getData(){return data;}
	inline const fixed *getData() const{return data;}

	inline bool equals(const Matrix3x3 &m2) const{
		return
			data[0]==m2.data[0] && data[1]==m2.data[1] && data[2]==m2.data[2] &&
			data[3]==m2.data[3] && data[4]==m2.data[4] && data[5]==m2.data[5] &&
			data[6]==m2.data[6] && data[7]==m2.data[7] && data[8]==m2.data[8];
	}

	inline bool operator==(const Matrix3x3 &m2) const{
		return equals(m2);
	}

	inline bool operator!=(const Matrix3x3 &m2) const{
		return !equals(m2);
	}

	Matrix3x3 operator*(const Matrix3x3 &m2) const;

	void operator*=(const Matrix3x3 &m2);

	Vector3 operator*(const Vector3 &v) const;
};

}
}
}

#endif

