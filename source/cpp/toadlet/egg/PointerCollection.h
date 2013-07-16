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
		inline iterator():it(NULL){}
		inline iterator(TypePtr *i):it(i){}

		inline iterator operator ++(){return ++it;}
		inline iterator operator ++(int){return it++;}
		inline iterator operator +(int i){return it+i;}
		inline iterator operator --(){return --it;}
		inline iterator operator --(int){return it--;}
		inline iterator operator -(int i){return it-i;}

		inline operator typename CollectionType::iterator() const{return it;}
		inline Type *operator*() const{return *it;}
		inline Type *operator->() const{return *it;}
		inline operator TypePtr() const{return *it;}
		inline operator Type*() const{return *it;}

		inline bool operator==(const iterator &i) const{return it==i.it;}
		inline bool operator!=(const iterator &i) const{return it!=i.it;}
		inline bool operator<(const iterator &i) const{return it<i.it;}
		inline bool operator>(const iterator &i) const{return it>i.it;}

		TypePtr *it;
	};

	class const_iterator{
	public:
		inline const_iterator():it(NULL){}
		inline const_iterator(const TypePtr *i):it(i){}

		inline const_iterator operator ++(){return ++it;}
		inline const_iterator operator ++(int){return it++;}
		inline const_iterator operator +(int i){return it+i;}
		inline const_iterator operator --(){return --it;}
		inline const_iterator operator --(int){return it--;}
		inline const_iterator operator -(int i){return it-i;}

		inline operator typename CollectionType::const_iterator() const{return it;}
		inline const Type *operator*() const{return *it;}
		inline const Type *operator->() const{return *it;}
		inline operator const TypePtr() const{return *it;}
		inline operator const Type*() const{return *it;}

		inline bool operator==(const const_iterator &i) const{return it==i.it;}
		inline bool operator!=(const const_iterator &i) const{return it!=i.it;}
		inline bool operator<(const const_iterator &i) const{return it<i.it;}
		inline bool operator>(const const_iterator &i) const{return it>i.it;}

		const TypePtr *it;
	};

	inline PointerCollection(){}

	inline PointerCollection(int size):mCollection(size){}

	inline void push_back(const TypePtr &type){mCollection.push_back(type);}

	inline void add(const TypePtr &type){mCollection.add(type);}

	inline void setAt(int index,const TypePtr &type){mCollection.setAt(index,type);}

	inline TypePtr back(){return mCollection.back();}

	inline void pop_back(){mCollection.pop_back();}

	inline iterator insert(iterator at,const TypePtr &type){return mCollection.insert(at,type);}

	inline iterator insert(iterator at,const_iterator start,const_iterator end){return mCollection.insert(at,start,end);}

	inline void resize(int s){mCollection.resize(s);}

	inline iterator erase(iterator it){return mCollection.erase(it);}

	template<typename Type2> inline bool remove(const Type2 &type){return mCollection.remove(type);}

	template<typename Type2> inline int indexOf(const Type2 &type) const{return mCollection.indexOf(type);}

	template<typename Type2> inline bool contains(const Type2 &type) const{return mCollection.contains(type);}

	inline void reserve(int s){mCollection.reserve(s);}

	inline int size() const{return mCollection.size();}

	inline int capacity() const{return mCollection.capacity();}
	
	inline bool empty() const{return mCollection.empty();}

	inline const iterator &begin(){return reinterpret_cast<const iterator&>(mCollection.begin());}
	inline const iterator &end(){return reinterpret_cast<const iterator&>(mCollection.end());}

	inline const const_iterator &begin() const{return reinterpret_cast<const const_iterator&>(mCollection.begin());}
	inline const const_iterator &end() const{return reinterpret_cast<const const_iterator&>(mCollection.end());}

	inline void clear(){mCollection.clear();}

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

