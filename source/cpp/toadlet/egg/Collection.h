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

#ifdef _WIN32
	#include "stlit/vector"
	#define TOADLET_STL_NAMESPACE stlit
#else
	#include <vector>
	#define TOADLET_STL_NAMESPACE std
#endif

#include "stlit/utility"

namespace toadlet{
namespace egg{

template <typename Type>
class Collection:public TOADLET_STL_NAMESPACE::vector<Type>{
public:
	inline Collection():TOADLET_STL_NAMESPACE::vector<Type>(){}
	inline Collection(int size):TOADLET_STL_NAMESPACE::vector<Type>(size){}
	inline Collection(int size,const Type &value):TOADLET_STL_NAMESPACE::vector<Type>(size,value){}
	inline Collection(const TOADLET_STL_NAMESPACE::vector<Type> &v):TOADLET_STL_NAMESPACE::vector<Type>(v){}
};

}
}

#undef TOADLET_STL_NAMESPACE

#endif
