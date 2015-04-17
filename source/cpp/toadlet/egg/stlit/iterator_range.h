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

#ifndef STLIT_ITERATOR_RANGE_H
#define STLIT_ITERATOR_RANGE_H

#include "iterator.h"

namespace stlit{

template<typename Type>
class abstract_iterator{
public:
	virtual ~abstract_iterator(){}

	virtual void increment()=0;
	virtual void decrement()=0;
	virtual abstract_iterator *clone() const=0;
	virtual Type dereference() const=0;
	virtual bool equals(abstract_iterator<Type> *it) const=0;
};

template<typename ValueType,typename IteratorType>
class wrap_iterator:public abstract_iterator<ValueType>{
public:
	wrap_iterator(IteratorType it):iter(it){}

	void increment(){++iter;}
	void decrement(){--iter;}
	abstract_iterator<ValueType> *clone() const{return new wrap_iterator(iter);}
	ValueType dereference() const{return *iter;}
	bool equals(abstract_iterator<ValueType> *it) const{return iter==((wrap_iterator*)it)->iter;}

	IteratorType iter;
};

template<typename Type>
class any_iterator{
public:
	typedef Type value_type;
	typedef Type& reference;
	typedef const Type& const_reference;
	typedef Type* pointer;
	typedef int difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;

	any_iterator(const abstract_iterator<Type> &it):iter(it.clone()){}
	any_iterator(const any_iterator &it):iter(it.iter->clone()){}
	any_iterator():iter(NULL){}
	~any_iterator(){delete iter;}

	inline Type operator*() const{return iter->dereference();}
	inline any_iterator& operator=(const any_iterator& it){delete iter;iter=it.iter->clone();return(*this);}
	inline any_iterator& operator++(){iter->increment();return(*this);}
	inline any_iterator operator++(int){any_iterator it=*this;iter->increment();return it;}
	inline any_iterator& operator--(){iter->decrement();return(*this);}

	inline bool operator==(const any_iterator<Type> &it) const{return iter->equals(it.iter);}
	inline bool operator!=(const any_iterator<Type> &it) const{return !iter->equals(it.iter);}

	abstract_iterator<Type> *iter;
};

template<typename Type>
class any_pointer_iterator{
public:
	typedef Type* value_type;
	typedef Type*& reference;
	typedef const Type*& const_reference;
	typedef Type** pointer;
	typedef int difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;

	any_pointer_iterator(const abstract_iterator<Type*> &it):iter(it.clone()){}
	any_pointer_iterator(const any_pointer_iterator &it):iter(it.iter->clone()){}
	any_pointer_iterator():iter(NULL){}
	~any_pointer_iterator(){delete iter;}

	inline Type *operator*() const{return iter->dereference();}
	inline Type *operator->() const{return iter->dereference();}
	inline operator Type*() const{return iter->dereference();}
	inline any_pointer_iterator& operator=(const any_pointer_iterator& it){delete iter;iter=it.iter->clone();return(*this);}
	inline any_pointer_iterator& operator++(){iter->increment();return(*this);}
	inline any_pointer_iterator operator++(int){any_pointer_iterator it=*this;iter->increment();return it;}
	inline any_pointer_iterator& operator--(){iter->decrement();return(*this);}

	inline bool operator==(const any_pointer_iterator<Type> &it) const{return iter->equals(it.iter);}
	inline bool operator!=(const any_pointer_iterator<Type> &it) const{return !iter->equals(it.iter);}

	abstract_iterator<Type*> *iter;
};

template<typename Type>
class iterator_range{
public:
	typedef any_iterator<Type> iterator;

	typedef typename iterator::value_type value_type;
	typedef typename iterator::reference reference;
	typedef typename iterator::const_reference const_reference;
	typedef typename iterator::pointer pointer;
	typedef typename iterator::difference_type difference_type;
	typedef typename iterator::iterator_category iterator_category;

	iterator_range(){}
	iterator_range(const iterator_range &range):beginit(range.beginit),endit(range.endit){}
	iterator_range(const iterator &bit,const iterator &eit):beginit(bit),endit(eit){}

	template<typename CollectionType>
	static iterator_range wrapCollection(const CollectionType &collection){return iterator_range(
		wrap_iterator<typename CollectionType::value_type,typename CollectionType::iterator>(collection.begin()),
		wrap_iterator<typename CollectionType::value_type,typename CollectionType::iterator>(collection.end()));}

	inline const iterator &begin() const{return beginit;}
	inline const iterator &end() const{return endit;}
	inline bool empty() const{return beginit==endit;}

	iterator beginit,endit;
};

template<typename Type>
class pointer_iterator_range{
public:
	typedef any_pointer_iterator<Type> iterator;

	typedef typename iterator::value_type value_type;
	typedef typename iterator::reference reference;
	typedef typename iterator::const_reference const_reference;
	typedef typename iterator::pointer pointer;
	typedef typename iterator::difference_type difference_type;
	typedef typename iterator::iterator_category iterator_category;

	pointer_iterator_range(){}
	pointer_iterator_range(const pointer_iterator_range &range):beginit(range.beginit),endit(range.endit){}
	pointer_iterator_range(const iterator &bit,const iterator &eit):beginit(bit),endit(eit){}

	template<typename CollectionType>
	static pointer_iterator_range wrapCollection(const CollectionType &collection){return pointer_iterator_range(
		wrap_iterator<Type*,typename CollectionType::iterator>(const_cast<CollectionType&>(collection).begin()),
		wrap_iterator<Type*,typename CollectionType::iterator>(const_cast<CollectionType&>(collection).end()));}

	inline const iterator &begin() const{return beginit;}
	inline const iterator &end() const{return endit;}
	inline bool empty() const{return beginit==endit;}

	iterator beginit,endit;
};

}

#endif

