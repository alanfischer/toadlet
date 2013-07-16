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

#include <toadlet/egg/Types.h>

namespace toadlet{
namespace egg{

template<typename Type>
class TOADLET_API Collection{
public:
	class const_iterator;

	class iterator{
	public:
		inline iterator():it(NULL){}
		inline iterator(Type *i):it(i){}
		inline explicit iterator(const const_iterator &i):it(const_cast<Type*>(i.it)){}

		inline iterator operator ++(){return ++it;}
		inline iterator operator ++(int){return it++;}
		inline iterator operator +(int i){return it+i;}
		inline iterator operator --(){return --it;}
		inline iterator operator --(int){return it--;}
		inline iterator operator -(int i){return it-i;}

		inline Type *operator*() const{return it;}
		inline Type *operator->() const{return it;}
		inline operator Type*() const{return it;}

		inline bool operator==(const iterator &i) const{return it==i.it;}
		inline bool operator!=(const iterator &i) const{return it!=i.it;}
		inline bool operator<(const iterator &i) const{return it<i.it;}
		inline bool operator>(const iterator &i) const{return it>i.it;}

		Type *it;
	};

	class const_iterator{
	public:
		inline const_iterator():it(NULL){}
		inline const_iterator(const Type *i):it(i){}
		inline explicit const_iterator(const iterator &i):it(i.it){}

		inline const_iterator operator ++(){return ++it;}
		inline const_iterator operator ++(int){return it++;}
		inline const_iterator operator +(int i){return it+i;}
		inline const_iterator operator --(){return --it;}
		inline const_iterator operator --(int){return it--;}
		inline const_iterator operator -(int i){return it-i;}

		inline const Type *operator*() const{return it;}
		inline const Type *operator->() const{return it;}
		inline operator const Type*() const{return it;}

		inline bool operator==(const const_iterator &i) const{return it==i.it;}
		inline bool operator!=(const const_iterator &i) const{return it!=i.it;}
		inline bool operator<(const const_iterator &i) const{return it<i.it;}
		inline bool operator>(const const_iterator &i) const{return it>i.it;}

		const Type *it;
	};

	inline Collection():
		mSize(0),
		mCapacity(0),
		mData(NULL),
		mBegin(NULL),mEnd(NULL)
	{}

	inline Collection(int size):
		mSize(size),
		mCapacity(size),
		mBegin(NULL),mEnd(NULL)
	{
		mData=new Type[mCapacity+1];
	}

	inline Collection(int size,const Type &value):
		mSize(size),
		mCapacity(size)
	{
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<size;++i){
			mData[i]=value;
		}
		mBegin=&mData[0];mEnd=&mData[mSize];
	}

	inline Collection(const Type *values,int size):
		mSize(size),
		mCapacity(size)
	{
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<size;++i){
			mData[i]=values[i];
		}
		mBegin=&mData[0];mEnd=&mData[mSize];
	}

	inline Collection(const Collection &c):
		mSize(c.mSize),
		mCapacity(c.mCapacity)
	{
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
		mBegin=&mData[0];mEnd=&mData[mSize];
	}

	template<typename Type2> inline Collection(const Collection<Type2> &c):
		mSize(c.mSize),
		mCapacity(c.mCapacity)
	{
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
		mBegin=&mData[0];mEnd=&mData[mSize];
	}

	~Collection(){
		delete[] mData;
	}

	inline void push_back(const Type &type){
		if(mSize+1>mCapacity){
			reserve((mSize+1)*2);
		}

		mData[mSize]=type;
		mSize++;
		mEnd=&mData[mSize];
	}

	inline void add(const Type &type){
		push_back(type);
	}

	inline void setAt(int index,const Type &type){
		mData[index]=type;
	}

	inline Type &back(){
		return mData[mSize-1];
	}

	inline const Type &back() const{
		return mData[mSize-1];
	}

	inline void pop_back(){
		mData[mSize-1]=Type();

		mSize--;
		mEnd=&mData[mSize];
	}

	iterator insert(iterator at,const Type &type){
		int iat=at-mData;

		if(mSize+1>mCapacity){
			reserve((mSize+1)*2);
		}

		int i;
		for(i=mSize;i>iat;--i){
			mData[i]=mData[i-1];
		}

		mData[iat]=type;
		mSize++;
		mEnd=&mData[mSize];

		return &mData[iat];
	}

	inline void insert(int index,const Type &type){
		insert(begin()+index,type);
	}

	inline void addAll(const Collection<Type> &collection){
		insert(end(),collection.begin(),collection.end());
	}

	inline void addAll(const Collection<Type> *collection){
		insert(end(),collection->begin(),collection->end());
	}

	iterator insert(iterator at,const_iterator start,const_iterator end){
		int iat=at-mData;

		int addsize=end-start;
		if(mSize+addsize>mCapacity){
			reserve(mSize+addsize);
		}

		int i;
		for(i=mSize;i>iat;--i){
			mData[i]=mData[i-addsize];
		}

		for(i=0;i<addsize;++i){
			mData[iat+i]=*(start+i);
		}

		mSize+=addsize;
		mEnd=&mData[mSize];

		return &mData[iat];
	}

	void resize(int s){
		if(s>mCapacity){
			reserve(s);
		}

		if(s<mSize){
			for(;mSize>s;--mSize){
				mData[mSize-1]=Type();
			}
		}
		else{
			mSize=s;
		}
		mEnd=&mData[mSize];
	}

	void resize(int s,const Type &type){
		if(s>mCapacity){
			reserve(s);
		}

		if(mSize<s){
			for(;mSize<s;++mSize){
				mData[mSize]=type;
			}
		}
		else if(s<mSize){
			for(;mSize>s;--mSize){
				mData[mSize-1]=Type();
			}
		}
		mEnd=&mData[mSize];
	}

	iterator erase(iterator it){
		int iit=it-begin();

		int i;
		for(i=iit+1;i<mSize;++i){
			mData[i-1]=mData[i];
		}
		mData[mSize-1]=Type();
		mSize--;
		mEnd=&mData[mSize];

		return &mData[iit];
	}

	template<typename Type2> bool remove(const Type2 &type){
		int i;
		for(i=0;i<mSize;++i){
			if(mData[i]==type){
				break;
			}
		}

		if(i<mSize){
			for(i++;i<mSize;++i){
				mData[i-1]=mData[i];
			}

			mData[mSize-1]=Type();
			mSize--;
			mEnd=&mData[mSize];
			
			return true;
		}
		else{
			return false;
		}
	}

	void removeAt(int i){
		if(i>=0 && i<mSize){
			for(i++;i<mSize;++i){
				mData[i-1]=mData[i];
			}

			mData[mSize-1]=Type();
			mSize--;
			mEnd=&mData[mSize];
		}
	}

	template<typename Type2> int indexOf(const Type2 &type) const{
		int i;
		for(i=0;i<mSize;++i){
			if(mData[i]==type){
				return i;
			}
		}
		return -1;
	}

	template<typename Type2> bool contains(const Type2 &type) const{
		return indexOf(type)>=0;
	}

	void reserve(int s){
		if(mCapacity<s){
			Type *data=new Type[s+1];
			int i;
			for(i=0;i<mSize;++i){
				data[i]=mData[i];
			}
			delete[] mData;
			mData=data;
			mBegin=&mData[0];mEnd=&mData[mSize];
		}
		mCapacity=s;
	}

	inline int size() const{
		return mSize;
	}

	inline int capacity() const{
		return mCapacity;
	}
	
	inline bool empty() const{
		return mSize==0;
	}

	inline const iterator &begin(){return mBegin;}
	inline const iterator &end(){return mEnd;}

	inline const const_iterator &begin() const{return reinterpret_cast<const const_iterator&>(mBegin);}
	inline const const_iterator &end() const{return reinterpret_cast<const const_iterator&>(mEnd);}

	void clear(){
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=Type();
		}

		mSize=0;
		mEnd=&mData[mSize];
	}

	inline Type *toArray(){
		return mData;
	}

	inline const Type *toArray() const{
		return mData;
	}

	inline Type &get(int n){
		return mData[n];
	}

	inline const Type &get(int n) const{
		return mData[n];
	}

	inline Type &at(int n){
		return mData[n];
	}

	inline const Type &at(int n) const{
		return mData[n];
	}
	
	inline Type &operator[](int n){
		return mData[n];
	}

	inline const Type &operator[](int n) const{
		return mData[n];
	}

	inline operator Type *(){
		return mData;
	}

	inline operator const Type *() const{
		return mData;
	}

	Collection &operator=(const Collection &c){
		mSize=c.mSize;
		mCapacity=c.mCapacity;
		delete[] mData;
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
		mBegin=&mData[0];mEnd=&mData[mSize];
		return *this;
	}

	inline bool equals(const Collection &c) const{
		if(mSize!=c.mSize){
			return false;
		}

		int i;
		for(i=0;i<mSize;++i){
			if(mData[i]!=c.mData[i]){
				return false;
			}
		}
		return true;
	}

	inline bool operator==(const Collection &c) const{
		return equals(c);
	}

	inline bool operator!=(const Collection &c) const{
		return !equals(c);
	}

protected:
	int mSize;
	int mCapacity;
	Type *mData;
	iterator mBegin,mEnd;
};

}
}

#endif

