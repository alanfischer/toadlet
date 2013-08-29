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
	class const_iterator;

	class iterator{
    public:
		iterator():parent(NULL),index(0){}
		iterator(CollectionType *p,int i):parent(p),index(i){}
		iterator(const iterator &it):parent(it.parent),index(it.index){}
		iterator(const const_iterator &it):parent(it.parent),index(it.index){}
		iterator(const typename CollectionType::iterator &it):parent(it.parent),index(it.index){}

		Type& operator*() const{return *(*parent)[index];}
		Type* operator->() const{return (*parent)[index];}
		operator Type*() const{return (*parent)[index];}
		iterator& operator++(){index++;return(*this);}
		iterator& operator--(){index--;return(*this);}
		iterator operator++(int){iterator it(*this); operator++(); return(it);}
		iterator operator--(int){iterator it(*this); operator--(); return(it);}
		iterator operator+(int i) const{return iterator(parent,index+i);}
		iterator operator-(int i) const{return iterator(parent,index-i);}

		int operator-(const iterator &it) const{return index-it.index;}
		bool operator<(const iterator &it) const{return index<it.index;}
		bool operator>(const iterator &it) const{return index>it.index;}
		bool operator==(const iterator &it) const{return parent==it.parent && index==it.index;}
		bool operator!=(const iterator &it) const{return parent!=it.parent || index!=it.index;}

		int index;
		CollectionType *parent;
	};

    class const_iterator{
    public:
		const_iterator():parent(NULL),index(0){}
		const_iterator(CollectionType *p,int i):parent(p),index(i){}
		const_iterator(const const_iterator &it):parent(it.parent),index(it.index){}
		const_iterator(const iterator &it):parent(it.parent),index(it.index){}
		const_iterator(const typename CollectionType::const_iterator &it):parent(it.parent),index(it.index){}

		const Type& operator*() const{return *(*parent)[index];}
		const Type* operator->() const{return (*parent)[index];}
		operator Type*() const{return const_cast<Type*>((*parent)[index].get());}
		const_iterator& operator++(){index++;return(*this);}
		const_iterator& operator--(){index--;return(*this);}
		const_iterator operator++(int){const_iterator it(*this); operator++(); return(it);}
		const_iterator operator--(int){const_iterator it(*this); operator--(); return(it);}
		const_iterator operator+(int i) const{return const_iterator(parent,index+i);}
		const_iterator operator-(int i) const{return const_iterator(parent,index-i);}

		int operator-(const const_iterator &it) const{return index-it.index;}
		bool operator<(const const_iterator &it) const{return index<it.index;}
		bool operator>(const const_iterator &it) const{return index>it.index;}
		bool operator==(const const_iterator &it) const{return parent==it.parent && index==it.index;}
		bool operator!=(const const_iterator &it) const{return parent!=it.parent || index!=it.index;}

		int index;
		CollectionType *parent;
	};

	inline PointerCollection(){}
	inline PointerCollection(int size):mCollection(size){}

	inline TypePtr front(){return mCollection.front();}
	inline TypePtr back(){return mCollection.back();}

	inline const iterator &begin(){return reinterpret_cast<const iterator&>(mCollection.begin());}
	inline const iterator &end(){return reinterpret_cast<const iterator&>(mCollection.end());}

	inline const const_iterator &begin() const{return reinterpret_cast<const const_iterator&>(mCollection.begin());}
	inline const const_iterator &end() const{return reinterpret_cast<const const_iterator&>(mCollection.end());}

	inline void push_back(const TypePtr &type){mCollection.push_back(type);}
	inline void pop_back(){mCollection.pop_back();}
	
	inline iterator insert(const iterator &at,const TypePtr &type){return mCollection.insert(reinterpret_cast<const CollectionType::iterator&>(at),type);}
	inline iterator insert(const iterator &at,const const_iterator &start,const const_iterator &end){return mCollection.insert(reinterpret_cast<const CollectionType::iterator&>(at),reinterpret_cast<const CollectionType::iterator&>(start),reinterpret_cast<const CollectionType::iterator&>(end));}

	inline void resize(int s){mCollection.resize(s);}
	inline iterator erase(iterator it){return mCollection.erase(it);}
	template<typename Type2> inline bool remove(const Type2 &type){return mCollection.remove(type);}
	template<typename Type2> inline int indexOf(const Type2 &type) const{return mCollection.indexOf(type);}
	template<typename Type2> inline bool contains(const Type2 &type) const{return mCollection.contains(type);}
	inline void reserve(int s){mCollection.reserve(s);}
	inline void clear(){mCollection.clear();}

	inline int size() const{return mCollection.size();}
	inline int capacity() const{return mCollection.capacity();}
	inline bool empty() const{return mCollection.empty();}

	inline void removeAt(int i){erase(begin()+i);}
	inline void add(const TypePtr &type){mCollection.add(type);}
	inline void setAt(int index,const TypePtr &type){mCollection.setAt(index,type);}
	inline void addAll(const PointerCollection<Type> &collection){insert(end(),collection.begin(),collection.end());}
	inline void insert(int index,const TypePtr &type){mCollection.insert(index,type);}

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

