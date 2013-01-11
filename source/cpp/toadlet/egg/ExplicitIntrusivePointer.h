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

#ifndef TOADLET_EGG_EXPLICITINTRUSIVEPOINTER_H
#define TOADLET_EGG_EXPLICITINTRUSIVEPOINTER_H

#include <toadlet/egg/IntrusivePointer.h>

namespace toadlet{
namespace egg{

#if(defined(TOADLET_COMPILER_VC5) || defined(TOADLET_COMPILER_VC6))
	template<typename Type,typename PointerSemantics=DefaultIntrusiveSemantics,typename Dummy=DefaultIntrusiveSemantics>
#else
	template<typename Type,typename PointerSemantics=DefaultIntrusiveSemantics>
#endif
class ExplicitIntrusivePointer{
public:
	explicit inline ExplicitIntrusivePointer(Type *pointer){
		mPointer=pointer;
		if(mPointer!=NULL){
			PointerSemantics::retainReference(mPointer);
		}
	}

	inline ExplicitIntrusivePointer(){
		mPointer=NULL;
	}

	template<typename Type2> inline ExplicitIntrusivePointer(const ExplicitIntrusivePointer<Type2,PointerSemantics> &pointer){
		mPointer=const_cast<Type2*>(pointer.get());
		if(mPointer!=NULL){
			PointerSemantics::retainReference(mPointer);
		}
	}

	// The specialized version of this function must come after the above one
	inline ExplicitIntrusivePointer(const ExplicitIntrusivePointer<Type,PointerSemantics> &pointer){
		mPointer=const_cast<Type*>(pointer.get());
		if(mPointer!=NULL){
			PointerSemantics::retainReference(mPointer);
		}
	}

	inline ~ExplicitIntrusivePointer(){
		cleanup();
	}

	template<typename Type2> ExplicitIntrusivePointer<Type,PointerSemantics> &operator=(const ExplicitIntrusivePointer<Type2,PointerSemantics> &pointer){
		if(this==(ExplicitIntrusivePointer<Type,PointerSemantics>*)&pointer){
			return *this;
		}

		cleanup();

		mPointer=const_cast<Type2*>(pointer.get());
		if(mPointer!=NULL){
			PointerSemantics::retainReference(mPointer);
		}

		return *this;
	}

	// The specialized version of this function must come after the above one
	inline ExplicitIntrusivePointer<Type,PointerSemantics> &operator=(const ExplicitIntrusivePointer<Type,PointerSemantics> &pointer){
		if(this==&pointer){
			return *this;
		}

		cleanup();

		mPointer=const_cast<Type*>(pointer.get());
		if(mPointer!=NULL){
			PointerSemantics::retainReference(mPointer);
		}

		return *this;
	}

	inline Type *get() const{
		return mPointer;
	}

	inline bool operator==(const ExplicitIntrusivePointer<Type,PointerSemantics> &pointer) const{
		return mPointer==pointer.mPointer;
	}

	inline bool operator!=(const ExplicitIntrusivePointer<Type,PointerSemantics> &pointer) const{
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
	inline bool operator<(const ExplicitIntrusivePointer<Type,PointerSemantics> &pointer) const{
		return get()<pointer.get();
	}

	inline void internal_setPointer(Type *pointer){
		mPointer=pointer;
	}

protected:
	void cleanup(){
		if(mPointer!=NULL){
			PointerSemantics::releaseReference(mPointer);
			mPointer=NULL;
		}
	}

	Type *mPointer;
};

template<typename Type,typename Type2,typename PointerSemantics> inline ExplicitIntrusivePointer<Type,PointerSemantics> shared_dynamic_cast(const ExplicitIntrusivePointer<Type2,PointerSemantics> &pointer){
	ExplicitIntrusivePointer<Type,PointerSemantics> p;
	p.internal_setPointer(dynamic_cast<Type*>(const_cast<Type2*>(pointer.get())));
	if(p.get()!=NULL){
		PointerSemantics::retainReference(p.get());
	}
	return p;
}

template<typename Type,typename Type2,typename PointerSemantics> inline ExplicitIntrusivePointer<Type,PointerSemantics> shared_static_cast(const ExplicitIntrusivePointer<Type2,PointerSemantics> &pointer){
	ExplicitIntrusivePointer<Type,PointerSemantics> p;
	p.internal_setPointer((Type*)pointer.get());
	if(p.get()!=NULL){
		PointerSemantics::retainReference(p.get());
	}
	return p;
}

}
}

#define TOADLET_IPTR(Class) \
	typedef toadlet::egg::IntrusivePointer<Class> ptr;

#endif
