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

#ifndef TOADLET_EGG_MAP_H
#define TOADLET_EGG_MAP_H

#ifdef _WIN32
	#include "stlit/map"
	#define TOADLET_STL_NAMESPACE stlit
#else
	#include <map>
	#define TOADLET_STL_NAMESPACE std
#endif

namespace toadlet{
namespace egg{

template <typename Type1,typename Type2>
class Pair:public TOADLET_STL_NAMESPACE::pair<Type1,Type2>{
public:
	inline Pair():TOADLET_STL_NAMESPACE::pair<Type1,Type2>(){}
	inline Pair(const Type1& a,const Type2& b):TOADLET_STL_NAMESPACE::pair<Type1,Type2>(a,b){}
};

template <typename Type1,typename Type2>
class Map:public TOADLET_STL_NAMESPACE::map<Type1,Type2>{
public:
	inline Map():TOADLET_STL_NAMESPACE::map<Type1,Type2>(){}
	inline Map(int size):TOADLET_STL_NAMESPACE::map<Type1,Type2>(size){}
	inline Map(const TOADLET_STL_NAMESPACE::map<Type1,Type2> &m):TOADLET_STL_NAMESPACE::map<Type1,Type2>(m){}
};

}
}

#undef TOADLET_STL_NAMESPACE

#endif
