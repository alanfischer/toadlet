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

#ifndef TOADLET_PEEPER_VARIABLEBUFFERFORMAT_H
#define TOADLET_PEEPER_VARIABLEBUFFERFORMAT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API VariableBufferFormat{
public:
	TOADLET_SHARED_POINTERS(VariableBufferFormat);

	enum Format{
		Format_UNKNOWN=			0,

		// Format types
		Format_MASK_TYPES=		0xFF,
		Format_TYPE_UINT_8=		1,
		Format_TYPE_INT_8=		2,
		Format_TYPE_INT_16=		3,
		Format_TYPE_INT_32=		4,
		Format_TYPE_FIXED_32=	5,
		Format_TYPE_FLOAT_32=	6,
		Format_TYPE_DOUBLE_64=	7,
		Format_TYPE_COLOR_RGBA=	8,

		// Format counts
		Format_SHIFT_COUNTS=	16,
		Format_MASK_COUNTS=		0xFF<<16,
		Format_COUNT_1=			1<<16,
		Format_COUNT_2=			2<<16,
		Format_COUNT_3=			3<<16,
		Format_COUNT_4=			4<<16,
		Format_COUNT_2X2=		5<<16,
		Format_COUNT_2X3=		6<<16,
		Format_COUNT_2X4=		7<<16,
		Format_COUNT_3X2=		8<<16,
		Format_COUNT_3X3=		9<<16,
		Format_COUNT_3X4=		10<<16,
		Format_COUNT_4X2=		11<<16,
		Format_COUNT_4X3=		12<<16,
		Format_COUNT_4X4=		13<<16,

		Format_BIT_TRANSPOSE=	1<<31,
	};

	class Variable{
	public:
		TOADLET_SHARED_POINTERS(Variable);

		Variable():
			mFormat(0),
			mOffset(0),
			mIndex(0),
			mArraySize(0)
		{}

		inline void setName(const String &name){mName=name;}
		inline const String &getName() const{return mName;}

		inline void setFullName(const String &name){mFullName=name;}
		inline const String &getFullName() const{return mFullName;}

		inline void setFormat(int format){mFormat=format;}
		inline int getFormat() const{return mFormat;}

		inline void setOffset(int offset){mOffset=offset;}
		inline int getOffset() const{return mOffset;}

		inline void setIndex(int index){mIndex=index;}
		inline int getIndex() const{return mIndex;}

		inline void setArraySize(int size){mArraySize=size;}
		inline int getArraySize() const{return mArraySize;}

		inline void setStructSize(int size){mStructVariables.resize(size);}
		inline int getStructSize() const{return mStructVariables.size();}

		inline void setStructVariable(int i,Variable::ptr variable){mStructVariables[i]=variable;}
		inline Variable::ptr getStructVariable(int i) const{return mStructVariables[i];}

	protected:
		String mName;
		String mFullName;
		int mFormat;
		int mOffset;
		int mIndex;
		int mArraySize;
		Collection<Variable::ptr> mStructVariables;
	};

	// static methods aren't allowed in Interfaces, but technically enums shouldn't be either, so these need to be separated to an alongside class
	static int getFormatSize(int format);
	static int getFormatRows(int format);
	static int getFormatColumns(int format);

	VariableBufferFormat(bool primary,const String &name,int dataSize,int numVariables):
		mPrimary(primary),
		mName(name),
		mDataSize(dataSize)
	{
		mStructVariable=Variable::ptr(new Variable());
		mStructVariable->setStructSize(numVariables);
	}

	inline void setPrimary(bool primary){mPrimary=primary;}
	inline bool getPrimary() const{return mPrimary;}

	inline void setName(const String &name){mName=name;}
	inline const String &getName() const{return mName;}

	inline void setDataSize(int size){mDataSize=size;}
	inline int getDataSize() const{return mDataSize;}

	inline void setStructSize(int size){mStructVariable->setStructSize(size);}
	inline int getStructSize() const{return mStructVariable->getStructSize();}

	inline void setStructVariable(int i,Variable::ptr variable){mStructVariable->setStructVariable(i,variable);}
	inline Variable::ptr getStructVariable(int i) const{return mStructVariable->getStructVariable(i);}

	inline int getSize() const{return mFlatVariables.size();}
	inline Variable::ptr getVariable(int i) const{return mFlatVariables[i];}

	void compile();

protected:
	void compile(Variable::ptr variable,Variable *parent);

	bool mPrimary;
	String mName;
	int mDataSize;
	Variable::ptr mStructVariable;
	Collection<Variable::ptr> mFlatVariables;
};

}
}

#endif
