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

#ifndef STLIT_CIRCULAR_BUFFER_H
#define STLIT_CIRCULAR_BUFFER_H

#include "vector.h"

namespace stlit{

template<typename Type>
class circular_buffer{
public:
	typedef Type value_type;
	typedef Type& reference;
	typedef const Type& const_reference;
	typedef Type* pointer;
	typedef int difference_type;
	typedef std::bidirectional_iterator_tag iterator_category;

	class iterator{
	public:
		typedef typename circular_buffer<Type>::value_type value_type;
		typedef typename circular_buffer<Type>::reference reference;
		typedef typename circular_buffer<Type>::const_reference const_reference;
		typedef typename circular_buffer<Type>::pointer pointer;
		typedef typename circular_buffer<Type>::difference_type difference_type;
		typedef typename circular_buffer<Type>::iterator_category iterator_category;

		inline iterator():it(NULL),begin(NULL),end(NULL){}
		inline iterator(Type *i,Type *b,Type *e):it(i),begin(b),end(e){}

		inline Type& operator*() const{return *it;}
		inline Type *operator->() const{return it;}
		inline operator Type*() const{return it;}
		inline const iterator &operator ++(){++it;if(it==end)it=begin;return *this;}
		inline iterator operator ++(int){iterator r=it++;if(it==end)it=begin;return r;}
		inline const iterator &operator --(){--it;if(it<begin)it=end-1;return *this;}
		inline iterator operator --(int){iterator r=it--;if(it<begin)it=end-1;return r;}

		inline bool operator==(const iterator &i) const{return it==i.it;}
		inline bool operator!=(const iterator &i) const{return it!=i.it;}

		Type *it;
		Type *begin,*end;
	};

	class const_iterator{
	public:
		typedef typename circular_buffer<Type>::value_type value_type;
		typedef typename circular_buffer<Type>::reference reference;
		typedef typename circular_buffer<Type>::const_reference const_reference;
		typedef typename circular_buffer<Type>::pointer pointer;
		typedef typename circular_buffer<Type>::difference_type difference_type;
		typedef typename circular_buffer<Type>::iterator_category iterator_category;

		inline const_iterator():it(NULL),begin(NULL),end(NULL){}
		inline const_iterator(const Type *i,const Type *b,const Type *e):it(i),begin(b),end(e){}

		inline const Type& operator*() const{return *it;}
		inline const Type *operator->() const{return it;}
		inline operator const Type*() const{return it;}
		inline const const_iterator &operator ++(){++it;if(it==end)it=begin;return *this;}
		inline const_iterator operator ++(int){iterator r=it++;if(it==end)it=begin;return r;}
		inline const const_iterator &operator --(){--it;if(it<begin)it=end-1;return *this;}
		inline const_iterator operator --(int){iterator r=it--;if(it<begin)it=end-1;return r;}

		inline bool operator==(const const_iterator &i) const{return it==i.it;}
		inline bool operator!=(const const_iterator &i) const{return it!=i.it;}

		const Type *it;
		const Type *begin,*end;
	};

	inline circular_buffer(int size):
		mSize(size+1),
		mBegin(0),
		mEnd(0),
		mContainer(mSize)
	{}

	inline Type &front(){return mContainer[mBegin];}
	inline Type &back(){return mContainer[(mEnd + mSize - 1) % mSize];}

	inline const Type &front() const{return mContainer[mBegin];}
	inline const Type &back() const{return mContainer[(mEnd + mSize - 1) % mSize];}

	inline iterator begin(){return iterator(&*(mContainer.begin()+mBegin),&*mContainer.begin(),&*mContainer.end());}
	inline iterator end(){return iterator(&*(mContainer.begin()+mEnd),&*mContainer.begin(),&*mContainer.end());}

	inline const_iterator begin() const{return iterator(&*(mContainer.begin()+mBegin),&*mContainer.begin(),&*mContainer.end());}
	inline const_iterator end() const{return iterator(&*(mContainer.begin()+mEnd),&*mContainer.begin(),&*mContainer.end());}

	inline void push_back(){
		mEnd = (mEnd + 1) % mSize;
		if(mEnd == mBegin){
			mBegin = (mBegin + 1) % mSize;
		}
	}

	inline void push_back(const Type &type){
		mContainer[mEnd]=type;
		mEnd = (mEnd + 1) % mSize;
		if(mEnd == mBegin){
			mBegin = (mBegin + 1) % mSize;
		}
	}

	inline void pop_front(){
		mBegin = (mBegin + 1) % mSize;
	}

	inline bool empty() const{return mEnd == mBegin; }
	inline bool full() const{return (mEnd + 1) % mSize == mBegin;}

protected:
	int mSize,mBegin,mEnd;
	vector<Type> mContainer;
};

}

#endif

