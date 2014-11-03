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

#ifndef TOADLET_EGG_COLLECTION_H
#define TOADLET_EGG_COLLECTION_H

#include "stlit/vector"
#include "stlit/utility"

namespace std{ using namespace stlit; }

namespace toadlet{
namespace egg{

template <typename Type>
class Collection:public std::vector<Type>{
public:
	inline Collection():std::vector<Type>(){}
	inline Collection(int size):std::vector<Type>(size){}
	inline Collection(int size,const Type &value):std::vector<Type>(size,value){}
	inline Collection(const std::vector<Type> &v):std::vector<Type>(v){}
};

}
}

#endif
