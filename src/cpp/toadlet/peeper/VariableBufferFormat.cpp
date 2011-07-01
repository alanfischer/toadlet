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

#include <toadlet/peeper/VariableBufferFormat.h>

namespace toadlet{
namespace peeper{

int VariableBufferFormat::getFormatSize(int format){
	int size=0;
	int type=(format&Format_MASK_TYPES);
	if(type==Format_TYPE_UINT_8 || type==Format_TYPE_INT_8){
		size=1;
	}
	else if(type==Format_TYPE_INT_16){
		size=2;
	}
	else if(type==Format_TYPE_INT_32 || type==Format_TYPE_FIXED_32 || type==Format_TYPE_FLOAT_32){
		size=4;
	}
	else if(type==Format_TYPE_DOUBLE_64){
		size=8;
	}
	else if(format==Format_TYPE_COLOR_RGBA){
		size=4;
	}

	int count=(format&Format_MASK_COUNTS);
	switch(count){
		case Format_COUNT_1:
			size*=1;
		break;
		case Format_COUNT_2:
			size*=2;
		break;
		case Format_COUNT_3:
			size*=3;
		break;
		case Format_COUNT_4:
			size*=4;
		break;
		case Format_COUNT_2X2:
			size*=4;
		break;
		case Format_COUNT_2X3:
			size*=6;
		break;
		case Format_COUNT_2X4:
			size*=8;
		break;
		case Format_COUNT_3X2:
			size*=6;
		break;
		case Format_COUNT_3X3:
			size*=9;
		break;
		case Format_COUNT_3X4:
			size*=12;
		break;
		case Format_COUNT_4X2:
			size*=8;
		break;
		case Format_COUNT_4X3:
			size*=12;
		break;
		case Format_COUNT_4X4:
			size*=16;
		break;
	}

	return size;
}

int VariableBufferFormat::getFormatRows(int format){
	switch(format&Format_MASK_COUNTS){
		case Format_COUNT_2X2:
		case Format_COUNT_2X3:
		case Format_COUNT_2X4:
			return 2;
		case Format_COUNT_3X2:
		case Format_COUNT_3X3:
		case Format_COUNT_3X4:
			return 3;
		case Format_COUNT_4X2:
		case Format_COUNT_4X3:
		case Format_COUNT_4X4:
			return 4;
		default:
			return 0;
	}
}

int VariableBufferFormat::getFormatColumns(int format){
	switch(format&Format_MASK_COUNTS){
		case Format_COUNT_2X2:
		case Format_COUNT_3X2:
		case Format_COUNT_4X2:
			return 2;
		case Format_COUNT_2X3:
		case Format_COUNT_3X3:
		case Format_COUNT_4X3:
			return 3;
		case Format_COUNT_2X4:
		case Format_COUNT_3X4:
		case Format_COUNT_4X4:
			return 4;
		default:
			return 0;
	}
}

void VariableBufferFormat::compile(){
	mFlatVariables.clear();
	int i;
	for(i=0;i<mStructVariable->getStructSize();++i){
		compile(mStructVariable->getStructVariable(i),mStructVariable);
	}
}

void VariableBufferFormat::compile(Variable::ptr variable,Variable *parent){
	mFlatVariables.add(variable);
	variable->setFullName(variable->getName());
	int i;
	for(i=0;i<variable->getStructSize();++i){
		compile(variable->getStructVariable(i),variable);
	}
}

}
}
