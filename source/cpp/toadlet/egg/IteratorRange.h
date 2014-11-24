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

#include "stlit/iterator_range.h"
#define TOADLET_STL_NAMESPACE stlit

namespace toadlet{
namespace egg{

template<typename Type>
class IteratorRange:public TOADLET_STL_NAMESPACE::iterator_range<Type>{
	IteratorRange():TOADLET_STL_NAMESPACE::iterator_range<Type>(){}
	IteratorRange(const TOADLET_STL_NAMESPACE::iterator_range<Type> &range):TOADLET_STL_NAMESPACE::iterator_range<Type>(range){}
	IteratorRange(const typename TOADLET_STL_NAMESPACE::iterator_range<Type>::iterator &bit,const typename TOADLET_STL_NAMESPACE::iterator_range<Type>::iterator &eit):TOADLET_STL_NAMESPACE::iterator_range<Type>(bit,eit){}
};

template<typename Type>
class PointerIteratorRange:public TOADLET_STL_NAMESPACE::pointer_iterator_range<Type>{
public:
	PointerIteratorRange():TOADLET_STL_NAMESPACE::pointer_iterator_range<Type>(){}
	PointerIteratorRange(const TOADLET_STL_NAMESPACE::pointer_iterator_range<Type> &range):TOADLET_STL_NAMESPACE::pointer_iterator_range<Type>(range){}
	PointerIteratorRange(const typename TOADLET_STL_NAMESPACE::pointer_iterator_range<Type>::iterator &bit,const typename TOADLET_STL_NAMESPACE::pointer_iterator_range<Type>::iterator &eit):TOADLET_STL_NAMESPACE::pointer_iterator_range<Type>(bit,eit){}

};

}
}

#undef TOADLET_STL_NAMESPACE

#endif

