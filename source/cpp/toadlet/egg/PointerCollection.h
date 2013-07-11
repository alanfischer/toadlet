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

#ifndef TOADLET_EGG_POINTERCOLLECTION_H
#define TOADLET_EGG_POINTERCOLLECTION_H

#include <toadlet/egg/Types.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/IntrusivePointer.h>
#include <toadlet/egg/ExplicitIntrusivePointer.h>

namespace toadlet{
namespace egg{

template<typename Type>
class TOADLET_API PointerCollection{
public:
	typedef typename Type::ptr TypePtr;
	typedef Collection<TypePtr> CollectionType;

	class iterator{
	public:
		iterator():it(NULL){}
		iterator(typename CollectionType::iterator i):it(i){}

		inline void operator ++(){++it;}
		inline void operator ++(int){it++;}

		inline operator typename CollectionType::iterator() const{return it;}
		inline Type *operator*() const{return **it;}
		inline Type *operator->() const{return **it;}
		inline operator TypePtr() const{return **it;}
		inline operator Type*() const{return **it;}

		typename CollectionType::iterator it;
	};

	class const_iterator{
	public:
		const_iterator():it(NULL){}
		const_iterator(typename CollectionType::const_iterator i):it(i){}

		inline void operator ++(){++it;}
		inline void operator ++(int){it++;}

		inline operator typename CollectionType::const_iterator() const{return it;}
		inline const Type *operator*() const{return **it;}
		inline const Type *operator->() const{return **it;}
		inline operator const TypePtr() const{return **it;}
		inline operator const Type*() const{return **it}

		typename CollectionType::const_iterator it;
	};

	PointerCollection(){}

	PointerCollection(int size):mCollection(size){}

	void push_back(const TypePtr &type){mCollection.push_back(type);}

	void add(const TypePtr &type){mCollection.add(type);}

	void setAt(int index,const TypePtr &type){mCollection.setAt(index,type);}

	TypePtr back(){return mCollection.back();}

	void pop_back(){mCollection.pop_back();}

	iterator insert(iterator at,const TypePtr &type){return mCollection.insert(at,type);}

	iterator insert(iterator at,const_iterator start,const_iterator end){return mCollection.insert(at,start,end);}

	void resize(int s){mCollection.resize(s);}

	iterator erase(iterator it){return mCollection.erase(it);}

	template<typename Type2> bool remove(const Type2 &type){return mCollection.remove(type);}

	template<typename Type2> int indexOf(const Type2 &type) const{return mCollection.indexOf(type);}

	template<typename Type2> bool contains(const Type2 &type) const{return mCollection.contains(type);}

	void reserve(int s){mCollection.reserve(s);}

	int size() const{return mCollection.size();}

	int capacity() const{return mCollection.capacity();}
	
	bool empty() const{return mCollection.empty();}

	iterator begin(){return mCollection.begin();}
	iterator end(){return mCollection.end();}

	const_iterator begin() const{return mCollection.begin();}
	const_iterator end() const{return mCollection.end();}

	void clear(){mCollection.clear();}

	inline TypePtr &get(int n){return mCollection.get(n);}
	inline const TypePtr &get(int n) const{return mCollection.get(n);}

	inline TypePtr &at(int n){return mCollection.at(n);}
	inline const TypePtr &at(int n) const{return mCollection.at(n);}

	inline TypePtr &operator[](int n){return mCollection[n];}
	inline const TypePtr &operator[](int n) const{return mCollection[n];}

protected:
	CollectionType mCollection;
};

}
}

#endif

