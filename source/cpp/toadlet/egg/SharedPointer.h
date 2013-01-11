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

#ifndef TOADLET_EGG_SHAREDPOINTER_H
#define TOADLET_EGG_SHAREDPOINTER_H

#include <toadlet/egg/Assert.h>
#include <toadlet/egg/PointerCounter.h>

namespace toadlet{
namespace egg{

class DefaultSharedSemantics{
public:
	template<typename Type> static int retainReference(PointerCounter *counter,Type *type){
		return counter->incSharedCount();
	}

	template<typename Type> static int releasePointer(PointerCounter *counter,Type *type){
		int count=counter->decSharedCount();
		if(count<1){
 			delete type;
		}
		return count;
	}
};

#if(defined(TOADLET_COMPILER_VC5) || defined(TOADLET_COMPILER_VC6))
	template<typename Type,typename PointerSemantics,typename Dummy=DefaultSharedSemantics>
#else
	template<typename Type,typename PointerSemantics>
#endif
class WeakPointer;

#if(defined(TOADLET_COMPILER_VC5) || defined(TOADLET_COMPILER_VC6))
	template<typename Type,typename PointerSemantics=DefaultSharedSemantics,typename Dummy=DefaultSharedSemantics>
#else
	template<typename Type,typename PointerSemantics=DefaultSharedSemantics>
#endif
class TOADLET_API SharedPointer{
	class NullType{};
public:
	explicit inline SharedPointer(Type *pointer){
		mPointer=pointer;
		mCount=new PointerCounter(1);
	}

	inline SharedPointer(){
		mPointer=NULL;
		mCount=NULL;
	}

	template<typename Type2> inline SharedPointer(const SharedPointer<Type2,PointerSemantics> &pointer){
		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}
	}

	// The specialized version of this function must come after the above one
	inline SharedPointer(const SharedPointer<Type,PointerSemantics> &pointer){
		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}
	}

	template<typename Type2> inline SharedPointer(const WeakPointer<Type2,PointerSemantics> &pointer){
		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}
	}

	// The specialized version of this function must come after the above one
	inline SharedPointer(const WeakPointer<Type,PointerSemantics> &pointer){
		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}
	}

	inline SharedPointer(NullType *null){
		TOADLET_ASSERT(NULL==null);
		mPointer=NULL;
		mCount=NULL;
	}

	inline ~SharedPointer(){
		cleanup();
	}

	template<typename Type2> SharedPointer<Type,PointerSemantics> &operator=(const SharedPointer<Type2,PointerSemantics> &pointer){
		if(this==(SharedPointer<Type,PointerSemantics>*)&pointer){
			return *this;
		}

		cleanup();

		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}

		return *this;
	}

	// The specialized version of this function must come after the above one
	inline SharedPointer<Type,PointerSemantics> &operator=(const SharedPointer<Type,PointerSemantics> &pointer){
		if(this==&pointer){
			return *this;
		}

		cleanup();

		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}

		return *this;
	}

	template<typename Type2> SharedPointer<Type,PointerSemantics> &operator=(const WeakPointer<Type2,PointerSemantics> &pointer){
		cleanup();

		mPointer=const_cast<Type2*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}

		return *this;
	}

	// The specialized version of this function must come after the above one
	inline SharedPointer<Type,PointerSemantics> &operator=(const WeakPointer<Type,PointerSemantics> &pointer){
		cleanup();

		mPointer=const_cast<Type*>(pointer.get());
		mCount=pointer.internal_getCount();
		if(mCount!=NULL){
			PointerSemantics::retainReference(mCount,mPointer);
		}

		return *this;
	}

	inline SharedPointer<Type,PointerSemantics> &operator=(NullType *null){
		TOADLET_ASSERT(NULL==null);
		cleanup();

		mPointer=NULL;
		mCount=NULL;

		return *this;
	}

	inline Type *get() const{
		return mPointer;
	}

	inline bool operator==(const SharedPointer<Type,PointerSemantics> &pointer) const{
		return mPointer==pointer.mPointer;
	}

	inline bool operator!=(const SharedPointer<Type,PointerSemantics> &pointer) const{
		return mPointer!=pointer.mPointer;
	}

	inline bool operator==(Type *pointer) const{
		return mPointer==pointer;
	}

	inline bool operator!=(Type *pointer) const{
		return mPointer!=pointer;
	}

	inline bool operator!() const{
		return mPointer==NULL;
	}

	inline bool isValid() const{
		return mPointer!=NULL;
	}

	inline operator Type*() const{
		return mPointer;
	}

	// By making the dereference operator templated with a dummy type, it allows us to make void SharedPointers
	template<typename Type2> inline Type2 &operator*() const{
		return *mPointer;
	}

	inline Type *operator->() const{
		return mPointer;
	}

	// For map useage
	inline bool operator<(const WeakPointer<Type,PointerSemantics> &pointer) const{
		return get()<pointer.get();
	}

	inline PointerCounter *internal_getCount() const{
		return mCount;
	}

	inline void internal_setPointer(Type *pointer){
		mPointer=pointer;
	}

	inline void internal_setCount(PointerCounter *count){
		mCount=count;
	}

protected:
	void cleanup(){
		if(mCount!=NULL){
			PointerSemantics::releasePointer(mCount,mPointer);
			mPointer=NULL;
			mCount=NULL;
		}
	}

	Type *mPointer;
	PointerCounter *mCount;
};

template<typename Type,typename Type2,typename PointerSemantics> inline SharedPointer<Type,PointerSemantics> shared_dynamic_cast(const SharedPointer<Type2,PointerSemantics> &pointer){
	SharedPointer<Type,PointerSemantics> p;
	p.internal_setPointer(dynamic_cast<Type*>(const_cast<Type2*>(pointer.get())));
	if(p.get()!=NULL){
		PointerCounter *count=pointer.internal_getCount();
		p.internal_setCount(count);
		if(count!=NULL){
			PointerSemantics::retainReference(count,p.get());
		}
	}
	return p;
}

template<typename Type,typename Type2,typename PointerSemantics> inline SharedPointer<Type,PointerSemantics> shared_dynamic_cast(const WeakPointer<Type2,PointerSemantics> &pointer){
	SharedPointer<Type,PointerSemantics> p;
	p.internal_setPointer(dynamic_cast<Type*>(const_cast<Type2*>(pointer.get())));
	if(p.get()!=NULL){
		PointerCounter *count=pointer.internal_getCount();
		p.internal_setCount(count);
		if(count!=NULL){
			PointerSemantics::retainReference(count,p.get());
		}
	}
	return p;
}

template<typename Type,typename Type2,typename PointerSemantics> inline SharedPointer<Type,PointerSemantics> shared_static_cast(const SharedPointer<Type2,PointerSemantics> &pointer){
	SharedPointer<Type,PointerSemantics> p;
	p.internal_setPointer((Type*)pointer.get());
	if(p.get()!=NULL){
		PointerCounter *count=pointer.internal_getCount();
		p.internal_setCount(count);
		if(count!=NULL){
			PointerSemantics::retainReference(count,p.get());
		}
	}
	return p;
}

template<typename Type,typename Type2,typename PointerSemantics> inline SharedPointer<Type,PointerSemantics> shared_static_cast(const WeakPointer<Type2,PointerSemantics> &pointer){
	SharedPointer<Type,PointerSemantics> p;
	p.internal_setPointer((Type*)pointer.get());
	if(p.get()!=NULL){
		PointerCounter *count=pointer.internal_getCount();
		p.internal_setCount(count);
		if(count!=NULL){
			PointerSemantics::retainReference(count,p.get());
		}
	}
	return p;
}

}
}

#define TOADLET_SPTR(Class) \
	typedef toadlet::egg::SharedPointer<Class> ptr;

#endif
