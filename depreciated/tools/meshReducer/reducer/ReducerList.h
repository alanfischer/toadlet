/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef REDUCER_REDUCERLIST_H
#define REDUCER_REDUCERLIST_H

#include <toadlet/egg/Collection.h>
#include <assert.h>
#include <algorithm>

namespace reducer{

template <class Type> class ReducerList{
public:
	// A thin wrapper around the Collection class with extra functions
	ReducerList(int s=0){mElements.resize(s);}
	~ReducerList(){;}

	int size(){return mElements.size();}
	void resize(int s){mElements.resize(s);}
	
	void add(Type t){mElements.push_back(t);}
	void addUnique(Type t);

	int contains(Type t);
	
	void pop(){mElements.pop_back();}
	void del(Type t);

	Type &operator[](int i){assert(i>=0 && i<size()); return mElements[i];}

	toadlet::egg::Collection<Type> mElements;
};

template <class Type>
void ReducerList<Type>::addUnique(Type t){
	if(!contains(t)){
		add(t);
	}
}

template <class Type>
int ReducerList<Type>::contains(Type t){
	typename toadlet::egg::Collection<Type>::iterator it;
	if((it=std::find(mElements.begin(),mElements.end(),t))!=mElements.end()){
		return 1;
	}
	else{
		return 0;
	}
}

template <class Type>
void ReducerList<Type>::del(Type t){
	typename toadlet::egg::Collection<Type>::iterator it;
	while((it=std::find(mElements.begin(),mElements.end(),t))!=mElements.end()){
		mElements.erase(it);
	}
}

}

#endif
