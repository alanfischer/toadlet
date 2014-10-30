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
#include <algorithm>

namespace std{ struct bidirectional_iterator_tag; }

namespace toadlet{
namespace egg{
#if 1
template<typename Type>
class TOADLET_API Collection{
public:
	typedef Type value_type;
	typedef Type& reference;
	typedef const Type& const_reference;
	typedef Type* pointer;
	typedef int difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;

	class const_iterator;

    class iterator{
    public:
		typedef typename Collection<Type>::value_type value_type;
		typedef typename Collection<Type>::reference reference;
		typedef typename Collection<Type>::const_reference const_reference;
		typedef typename Collection<Type>::pointer pointer;
		typedef typename Collection<Type>::difference_type difference_type;
		typedef typename Collection<Type>::iterator_category iterator_category;

		inline iterator():parent(NULL),index(0){}
		inline iterator(Collection *p,int i):parent(p),index(i){}
		inline iterator(const iterator &it):parent(it.parent),index(it.index){}
		inline iterator(const const_iterator &it):parent(it.parent),index(it.index){}

		inline Type& operator*() const{return (*parent)[index];}
		inline Type* operator->() const{return &(*parent)[index];}
		inline operator Type*() const{return &(*parent)[index];}
		inline iterator& operator++(){index++;return(*this);}
		inline iterator& operator--(){index--;return(*this);}
		inline iterator operator++(int){iterator it(*this); operator++(); return(it);}
		inline iterator operator--(int){iterator it(*this); operator--(); return(it);}
		inline iterator operator+(int i) const{return iterator(parent,index+i);}
		inline iterator operator-(int i) const{return iterator(parent,index-i);}

		inline int operator-(const iterator &it) const{return index-it.index;}
		inline bool operator<(const iterator &it) const{return index<it.index;}
		inline bool operator>(const iterator &it) const{return index>it.index;}
		inline bool operator==(const iterator &it) const{return parent==it.parent && index==it.index;}
		inline bool operator!=(const iterator &it) const{return parent!=it.parent || index!=it.index;}

		Collection *parent;
		int index;
	};

    class const_iterator{
    public:

		typedef typename Collection<Type>::value_type value_type;
		typedef typename Collection<Type>::reference reference;
		typedef typename Collection<Type>::const_reference const_reference;
		typedef typename Collection<Type>::pointer pointer;
		typedef typename Collection<Type>::difference_type difference_type;
		typedef typename Collection<Type>::iterator_category iterator_category;

		inline const_iterator():parent(NULL),index(0){}
		inline const_iterator(Collection *p,int i):parent(p),index(i){}
		inline const_iterator(const const_iterator &it):parent(it.parent),index(it.index){}
		inline const_iterator(const iterator &it):parent(it.parent),index(it.index){}

		inline const Type& operator*() const{return (*parent)[index];}
		inline const Type* operator->() const{return &(*parent)[index];}
		inline operator const Type*() const{return &(*parent)[index];}
		inline const_iterator& operator++(){index++;return(*this);}
		inline const_iterator& operator--(){index--;return(*this);}
		inline const_iterator operator++(int){const_iterator it(*this); operator++(); return(it);}
		inline const_iterator operator--(int){const_iterator it(*this); operator--(); return(it);}
		inline const_iterator operator+(int i) const{return const_iterator(parent,index+i);}
		inline const_iterator operator-(int i) const{return const_iterator(parent,index-i);}

		inline int operator-(const const_iterator &it) const{return index-it.index;}
		inline bool operator<(const const_iterator &it) const{return index<it.index;}
		inline bool operator>(const const_iterator &it) const{return index>it.index;}
		inline bool operator==(const const_iterator &it) const{return parent==it.parent && index==it.index;}
		inline bool operator!=(const const_iterator &it) const{return parent!=it.parent || index!=it.index;}

		Collection *parent;
		int index;
	};

	inline Collection():
		mSize(0),
		mCapacity(0),
		mData(NULL)
	{
		mBegin=iterator(this,0);mEnd=iterator(this,0);
	}

	inline Collection(int size):
		mSize(0),
		mCapacity(0),
		mData(NULL)
	{
		resize(size);
		mBegin = iterator(this, 0);
	}

	inline Collection(int size,const Type &value):
		mSize(0),
		mCapacity(0),
		mData(NULL)
	{
		resize(size);
		int i;
		for(i=0;i<size;++i){
			mData[i]=value;
		}
		mBegin=iterator(this,0);
	}

	inline Collection(const Type *values,int size):
		mSize(0),
		mCapacity(0),
		mData(NULL)
	{
		resize(size);
		int i;
		for(i=0;i<size;++i){
			mData[i]=values[i];
		}
		mBegin=iterator(this,0);
	}

	inline Collection(const Collection &c):
		mSize(0),
		mCapacity(0),
		mData(NULL)
	{
		reserve(c.mCapacity);
		resize(c.mSize);
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
		mBegin=iterator(this,0);
	}

	template<typename Type2> inline Collection(const Collection<Type2> &c):
		mSize(0),
		mCapacity(0),
		mData(NULL)
	{
		reserve(c.mCapacity);
		resize(c.mSize);
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
		mBegin=iterator(this,0);
	}

	~Collection(){
		int i;
		for(i=mSize-1;i>=0;--i){
			mData[i].~Type();
		}
		TOADLET_ALIGNED_FREE(mData);
	}

	inline Type &front(){return mData[0];}
	inline Type &back(){return mData[mSize-1];}

	inline const Type &front() const{return mData[0];}
	inline const Type &back() const{return mData[mSize-1];}

	inline const iterator &begin(){return mBegin;}
	inline const iterator &end(){return mEnd;}

	inline const const_iterator &begin() const{return reinterpret_cast<const const_iterator&>(mBegin);}
	inline const const_iterator &end() const{return reinterpret_cast<const const_iterator&>(mEnd);}

	inline void push_back(const Type &type){
		if(mSize+1>mCapacity){
			reserve((mSize+1)*2);
		}

		mData[mSize]=type;
		mSize++;
		mEnd=iterator(this,mSize);
	}

	inline void pop_back(){
		mData[mSize-1]=Type();

		mSize--;
		mEnd=iterator(this,mSize);
	}

	iterator insert(const iterator &at,const Type &type){
		int iat=at-begin();

		if(mSize+1>mCapacity){
			reserve((mSize+1)*2);
		}

		int i;
		for(i=mSize;i>iat;--i){
			mData[i]=mData[i-1];
		}

		mData[iat]=type;
		mSize++;
		mEnd=iterator(this,mSize);

		return iterator(this,iat);
	}

	iterator insert(const iterator &at,const const_iterator &start,const const_iterator &end){
		int iat=at-mBegin;

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
		mEnd=iterator(this,mSize);

		return iterator(this,iat);
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
		mEnd=iterator(this,mSize);
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
		mEnd=iterator(this,mSize);
	}

	iterator erase(const iterator &it){
		int iit=it-begin();

		int i;
		for(i=iit+1;i<mSize;++i){
			mData[i-1]=mData[i];
		}
		mData[mSize-1]=Type();
		mSize--;
		mEnd=iterator(this,mSize);

		return iterator(this,iit);
	}

	iterator erase(const iterator &start,const iterator &end){
		int istart=start-begin();
		int size=end-start;

		int i;
		for(i=istart+size;i<mSize;++i){
			mData[i-size]=mData[i];
		}
		for(i=mSize-size;i<mSize;++i){
			mData[i]=Type();
		}
		mSize-=size;
		mEnd=iterator(this,mSize);

		return iterator(this,istart);
	}

	void reserve(int s){
		if(mCapacity<s){
			Type *data=(Type*)TOADLET_ALIGNED_MALLOC(sizeof(Type)*(s+1),TOADLET_ALIGNED_SIZE);

			int i;
			for(i=0;i<s;++i){
				new (&data[i]) Type;
			}

			for(i=0;i<mSize;++i){
				data[i]=mData[i];
			}

			for(i=0;i<mCapacity;++i){
				mData[i].~Type();
			}

			TOADLET_ALIGNED_FREE(mData);

			mData=data;
			mBegin=iterator(this,0);mEnd=iterator(this,mSize);
		}
		mCapacity=s;
	}

	void clear(){
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=Type();
		}

		mSize=0;
		mEnd=iterator(this,mSize);
	}

	inline Type *data() const{return mData;}
	inline int size() const{return mSize;}
	inline int capacity() const{return mCapacity;}
	inline bool empty() const{return mSize==0;}

	inline Type &at(int n){return mData[n];}
	inline const Type &at(int n) const{return mData[n];}
	
	inline Type &operator[](int n){return mData[n];}
	inline const Type &operator[](int n) const{return mData[n];}

	Collection &operator=(const Collection &c){
		reserve(c.mCapacity);
		resize(c.mSize);
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
		mBegin=iterator(this,0);mEnd=iterator(this,mSize);
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

	inline bool operator==(const Collection &c) const{return equals(c);}
	inline bool operator!=(const Collection &c) const{return !equals(c);}

protected:
	int mSize;
	int mCapacity;
	Type *mData;
	iterator mBegin,mEnd;
};
#else
#define Collection std::vector
#endif

}
}

#endif

