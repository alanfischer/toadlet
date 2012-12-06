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
#include <toadlet/egg/SharedPointer.h>

namespace toadlet{
namespace egg{

template<typename Type>
class TOADLET_API Collection{
public:
	TOADLET_SPTR(Collection);

	typedef Type* iterator;
	typedef const Type* const_iterator;

	Collection(){
		mSize=0;
		mCapacity=0;
		mData=NULL;
	}

	Collection(int size){
		mSize=size;
		mCapacity=size;
		mData=new Type[mCapacity+1];
	}

	Collection(int size,const Type &value){
		mSize=size;
		mCapacity=size;
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<size;++i){
			mData[i]=value;
		}
	}

	Collection(const Collection &c){
		mSize=c.mSize;
		mCapacity=c.mCapacity;
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
	}

	Collection(const Collection *c){
		mSize=c->mSize;
		mCapacity=c->mCapacity;
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c->mData[i];
		}
	}

	template<typename Type2> Collection(const Collection<Type2> &c){
		mSize=c.mSize;
		mCapacity=c.mCapacity;
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c.mData[i];
		}
	}

	template<typename Type2> Collection(const Collection<Type2> *c){
		mSize=c->mSize;
		mCapacity=c->mCapacity;
		mData=new Type[mCapacity+1];
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=c->mData[i];
		}
	}

	~Collection(){
		if(mData!=NULL){
			delete[] mData;
			mData=NULL;
		}
	}

	void push_back(const Type &type){
		if(mSize+1>mCapacity){
			reserve((mSize+1)*2);
		}

		mData[mSize]=type;
		mSize++;
	}

	void add(const Type &type){
		push_back(type);
	}

	void setAt(int index,const Type &type){
		mData[index]=type;
	}

	const Type &back(){
		return mData[mSize-1];
	}

	void pop_back(){
		mData[mSize-1]=Type();

		mSize--;
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

		return &mData[iat];
	}

	void insert(int index,const Type &type){
		insert(begin()+index,type);
	}

	void addAll(const Collection<Type> &collection){
		insert(end(),collection.begin(),collection.end());
	}

	void addAll(const Collection<Type> *collection){
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
	}

	iterator erase(iterator it){
		int iit=it-mData;

		int i;
		for(i=iit+1;i<mSize;++i){
			mData[i-1]=mData[i];
		}
		mData[mSize-1]=Type();
		mSize--;

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
		}
	}

	template<typename Type2> int indexOf(const Type2 &type){
		int i;
		for(i=0;i<mSize;++i){
			if(mData[i]==type){
				return i;
			}
		}
		return -1;
	}

	template<typename Type2> bool contains(const Type2 &type){
		return indexOf(type)>=0;
	}

	void reserve(int s){
		if(mCapacity<s){
			Type *data=new Type[s];
			int i;
			for(i=0;i<mSize;++i){
				data[i]=mData[i];
			}
			delete[] mData;
			mData=data;
		}
		mCapacity=s;
	}

	int size() const{
		return mSize;
	}

	int capacity() const{
		return mCapacity;
	}
	
	bool empty() const{
		return mSize==0;
	}

	iterator begin(){
		return &mData[0];
	}

	const iterator begin() const{
		return &mData[0];
	}

	iterator end(){
		return &mData[mSize];
	}

	const Type *end() const{
		return &mData[mSize];
	}

	void clear(){
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=Type();
		}

		mSize=0;
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
		return *this;
	}

	int mSize;
	int mCapacity;
	Type *mData;
};

}
}

#endif

