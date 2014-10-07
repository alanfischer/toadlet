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

#ifndef TOADLET_EGG_ITERATOR_H
#define TOADLET_EGG_ITERATOR_H

#include <toadlet/egg/Types.h>

namespace toadlet{
namespace egg{

template<typename Type>
class Iterator{
public:
	virtual ~Iterator(){}

	virtual void increment()=0;
	virtual void decrement()=0;
	virtual Iterator *clone() const=0;
	virtual Type dereference() const=0;
	virtual bool equals(Iterator<Type> *it) const=0;
};

template<typename ValueType,typename IteratorType>
class WrapIterator:public Iterator<ValueType>{
public:
	WrapIterator(IteratorType it):iter(it){}

	void increment(){++iter;}
	void decrement(){--iter;}
	Iterator<ValueType> *clone() const{return new WrapIterator(iter);}
	ValueType dereference() const{return *iter;}
	bool equals(Iterator<ValueType> *it) const{return iter==((WrapIterator*)it)->iter;}

	IteratorType iter;
};

template<typename PointerType,typename IteratorType>
class WrapPointerIterator:public Iterator<PointerType>{
public:
	WrapPointerIterator(IteratorType it):iter(it){}

	void increment(){++iter;}
	void decrement(){--iter;}
	Iterator<PointerType> *clone() const{return new WrapPointerIterator(iter);}
	PointerType dereference() const{return &*iter;}
	bool equals(Iterator<PointerType> *it) const{return iter==((WrapPointerIterator*)it)->iter;}

	IteratorType iter;
};

template<typename Type>
class AnyIterator{
public:
	AnyIterator(const Iterator<Type> &it):iter(it.clone()){}
	AnyIterator(const AnyIterator &it):iter(it.iter->clone()){}
	AnyIterator():iter(NULL){}
	~AnyIterator(){delete iter;}

	inline Type operator*() const{return iter->dereference();}
	inline AnyIterator& operator=(const AnyIterator& it){delete iter;iter=it.iter->clone();return(*this);}
	inline AnyIterator& operator++(){iter->increment();return(*this);}
	inline AnyIterator operator++(int){AnyIterator it=*this;iter->increment();return it;}
	inline AnyIterator& operator--(){iter->decrement();return(*this);}

	inline bool operator==(const AnyIterator<Type> &it) const{return iter->equals(it.iter);}
	inline bool operator!=(const AnyIterator<Type> &it) const{return !iter->equals(it.iter);}

	Iterator<Type> *iter;
};

template<typename Type>
class AnyPointerIterator{
public:
	AnyPointerIterator(const Iterator<Type*> &it):iter(it.clone()){}
	AnyPointerIterator(const AnyPointerIterator &it):iter(it.iter->clone()){}
	AnyPointerIterator():iter(NULL){}
	~AnyPointerIterator(){delete iter;}

	inline Type *operator*() const{return iter->dereference();}
	inline Type *operator->() const{return iter->dereference();}
	inline operator Type*() const{return iter->dereference();}
	inline AnyPointerIterator& operator=(const AnyPointerIterator& it){delete iter;iter=it.iter->clone();return(*this);}
	inline AnyPointerIterator& operator++(){iter->increment();return(*this);}
	inline AnyPointerIterator operator++(int){AnyPointerIterator it=*this;iter->increment();return it;}
	inline AnyPointerIterator& operator--(){iter->decrement();return(*this);}

	inline bool operator==(const AnyPointerIterator<Type> &it) const{return iter->equals(it.iter);}
	inline bool operator!=(const AnyPointerIterator<Type> &it) const{return !iter->equals(it.iter);}

	Iterator<Type*> *iter;
};

template<typename Type>
class IteratorRange{
public:
	typedef Type value_type;
	typedef AnyIterator<Type> iterator;
	typedef Type& reference;
	typedef const Type& const_reference;

	IteratorRange(){}
	IteratorRange(const IteratorRange &range):beginit(range.beginit),endit(range.endit){}
	IteratorRange(const iterator &bit,const iterator &eit):beginit(bit),endit(eit){}

	template<typename CollectionType>
	static IteratorRange wrapCollection(const CollectionType &collection){return IteratorRange(
		WrapIterator<typename CollectionType::value_type,typename CollectionType::iterator>(collection.begin()),
		WrapIterator<typename CollectionType::value_type,typename CollectionType::iterator>(collection.end()));}

	inline const iterator &begin() const{return beginit;}
	inline const iterator &end() const{return endit;}

	iterator beginit,endit;
};

template<typename Type>
class PointerIteratorRange{
public:
	typedef typename Type* value_type;
	typedef AnyPointerIterator<Type> iterator;
	typedef typename Type*& reference;
	typedef const typename Type*& const_reference;

	PointerIteratorRange(){}
	PointerIteratorRange(const PointerIteratorRange &range):beginit(range.beginit),endit(range.endit){}
	PointerIteratorRange(const iterator &bit,const iterator &eit):beginit(bit),endit(eit){}

	template<typename CollectionType>
	static PointerIteratorRange wrapCollection(const CollectionType &collection){return PointerIteratorRange(
		WrapIterator<Type*,typename CollectionType::iterator>(collection.begin()),
		WrapIterator<Type*,typename CollectionType::iterator>(collection.end()));}

	template<typename CollectionType>
	static PointerIteratorRange wrapPointerCollection(const CollectionType &collection){return PointerIteratorRange(
		WrapPointerIterator<Type*,typename CollectionType::iterator>(collection.begin()),
		WrapPointerIterator<Type*,typename CollectionType::iterator>(collection.end()));}

	inline const iterator &begin() const{return beginit;}
	inline const iterator &end() const{return endit;}

	iterator beginit,endit;
};

}
}

#endif

