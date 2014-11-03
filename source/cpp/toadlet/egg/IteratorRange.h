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

#ifndef TOADLET_EGG_ITERATORRANGE_H
#define TOADLET_EGG_ITERATORRANGE_H

#include "stlit/iterator_range"

namespace std{ using namespace stlit; }

namespace toadlet{
namespace egg{

template<typename Type>
class IteratorRange:public std::iterator_range<Type>{
	IteratorRange():std::iterator_range<Type>(){}
	IteratorRange(const std::iterator_range<Type> &range):std::iterator_range<Type>(range){}
	IteratorRange(const typename std::iterator_range<Type>::iterator &bit,const typename std::iterator_range<Type>::iterator &eit):std::iterator_range<Type>(bit,eit){}
};

template<typename Type>
class PointerIteratorRange:public std::pointer_iterator_range<Type>{
public:
	PointerIteratorRange():std::pointer_iterator_range<Type>(){}
	PointerIteratorRange(const std::pointer_iterator_range<Type> &range):std::pointer_iterator_range<Type>(range){}
	PointerIteratorRange(const typename std::pointer_iterator_range<Type>::iterator &bit,const typename std::pointer_iterator_range<Type>::iterator &eit):std::pointer_iterator_range<Type>(bit,eit){}

};

}
}

#endif

