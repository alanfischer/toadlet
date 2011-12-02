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

#ifndef TOADLET_EGG_WEAKPOINTER_H
#define TOADLET_EGG_WEAKPOINTER_H

#include <toadlet/egg/SharedPointer.h>
#include <toadlet/egg/IntrusivePointer.h>

namespace toadlet{
namespace egg{

#if(defined(TOADLET_COMPILER_VC5) || defined(TOADLET_COMPILER_VC6))
	template<typename Type,typename PointerSemantics,typename Dummy>
#else
	template<typename Type,typename PointerSemantics>
#endif
class TOADLET_API WeakPointer{
public:
	inline WeakPointer(){
		mPointer=NULL;
		mCount=NULL;
	}

	template<typename Type2> inline WeakPointer(const SharedPointer<Type2,PointerSemantics> &pointer){
		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount){
			mCount->incTotalCount();
		}
	}

	// The specialized version of this function must come after the above one
	inline WeakPointer(const SharedPointer<Type,PointerSemantics> &pointer){
		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount){
			mCount->incTotalCount();
		}
	}

	template<typename Type2> inline WeakPointer(const IntrusivePointer<Type2,PointerSemantics> &pointer){
		mPointer=const_cast<Type2*>(pointer.get());
		if(mPointer!=NULL){
			mCount=PointerSemantics::getCount(mPointer);
			mCount->incTotalCount();
		}
		else{
			mCount=NULL;
		}
	}

	// The specialized version of this function must come after the above one
	inline WeakPointer(const IntrusivePointer<Type,PointerSemantics> &pointer){
		mPointer=const_cast<Type*>(pointer.get());
		if(mPointer!=NULL){
			mCount=PointerSemantics::getCount(mPointer);
			mCount->incTotalCount();
		}
		else{
			mCount=NULL;
		}
	}

	template<typename Type2> inline WeakPointer(const WeakPointer<Type2,PointerSemantics> &pointer){
		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount){
			mCount->incTotalCount();
		}
	}

	// The specialized version of this function must come after the above one
	inline WeakPointer(const WeakPointer<Type,PointerSemantics> &pointer){
		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount){
			mCount->incTotalCount();
		}
	}

	inline ~WeakPointer(){
		cleanup();
	}

#	if(defined(TOADLET_COMPILER_VC5) || defined(TOADLET_COMPILER_VC6))
		inline WeakPointer<Type,PointerSemantics> &operator=(const int null){
			Error__Cannot_assign_pointer_to_WeakPointer;
		}
#	endif

	template<typename Type2> inline WeakPointer<Type,PointerSemantics> &operator=(const SharedPointer<Type2,PointerSemantics> &pointer){
		cleanup();

		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount){
			mCount->incTotalCount();
		}

		return *this;
	}

	// The specialized version of this function must come after the above one
	inline WeakPointer<Type,PointerSemantics> &operator=(const SharedPointer<Type,PointerSemantics> &pointer){
		cleanup();

		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount){
			mCount->incTotalCount();
		}

		return *this;
	}

	template<typename Type2> inline WeakPointer<Type,PointerSemantics> &operator=(const IntrusivePointer<Type2,PointerSemantics> &pointer){
		cleanup();

		mPointer=const_cast<Type2*>(pointer.get());
		if(mPointer!=NULL){
			mCount=PointerSemantics::getCount(mPointer);
			mCount->incTotalCount();
		}
		else{
			mCount=NULL;
		}

		return *this;
	}

	// The specialized version of this function must come after the above one
	inline WeakPointer<Type,PointerSemantics> &operator=(const IntrusivePointer<Type,PointerSemantics> &pointer){
		cleanup();

		mPointer=const_cast<Type*>(pointer.get());
		if(mPointer!=NULL){
			mCount=PointerSemantics::getCount(mPointer);
			mCount->incTotalCount();
		}
		else{
			mCount=NULL;
		}

		return *this;
	}

	template<typename Type2> inline WeakPointer<Type,PointerSemantics> &operator=(const WeakPointer<Type2,PointerSemantics> &pointer){
		if(mPointer==pointer.get()){
			return *this;
		}

		cleanup();

		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.mCount;
		if(mCount){
			mCount->incTotalCount();
		}

		return *this;
	}

	// The specialized version of this function must come after the above one
	inline WeakPointer<Type,PointerSemantics> &operator=(const WeakPointer<Type,PointerSemantics> &pointer){
		if(mPointer==pointer.get()){
			return *this;
		}

		cleanup();

		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.mCount;
		if(mCount){
			mCount->incTotalCount();
		}

		return *this;
	}

	inline Type *get() const{
		if(mCount==NULL || mCount->getSharedCount()<1){
			return NULL;
		}
		else{
			return mPointer;
		}
	}

	inline bool operator==(const WeakPointer<Type,PointerSemantics> &pointer) const{
		return get()==pointer.get();
	}

	inline bool operator!=(const WeakPointer<Type,PointerSemantics> &pointer) const{
		return get()!=pointer.get();
	}

	inline bool operator==(Type *pointer) const{
		return mPointer==pointer;
	}

	inline bool operator!=(Type *pointer) const{
		return mPointer!=pointer;
	}

	inline bool operator!() const{
		return get()==NULL;
	}

	inline bool isValid() const{
		return get()!=NULL;
	}

	inline operator Type*() const{
		return get();
	}

	// By making the dereference operator templated with a dummy type, it allows us to make void WeakPointers
	template<typename Type2> inline Type2 &operator*() const{
		return *get();
	}

	inline Type *operator->() const{
		return get();
	}

	// For map useage
	inline bool operator<(const WeakPointer<Type,PointerSemantics> &pointer) const{
		return get()<pointer.get();
	}

	inline PointerCounter *internal_getCount() const{
		return mCount;
	}

protected:
	void cleanup(){
		if(mCount==NULL){
			return;
		}

		if(mCount->decTotalCount()<1){
			mCount=NULL;
		}
	}

	Type *mPointer;
	PointerCounter *mCount;
};

}
}

#define TOADLET_SHARED_POINTERS(Class) \
	typedef toadlet::egg::SharedPointer<Class> ptr; \
	typedef toadlet::egg::WeakPointer<Class,toadlet::egg::DefaultSharedSemantics> wptr

#define TOADLET_INTRUSIVE_POINTERS(Class) \
	typedef toadlet::egg::IntrusivePointer<Class> ptr; \
	typedef toadlet::egg::WeakPointer<Class,toadlet::egg::DefaultIntrusiveSemantics> wptr

#endif
