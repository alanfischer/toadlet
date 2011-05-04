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

#ifndef TOADLET_EGG_TYPE_H
#define TOADLET_EGG_TYPE_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Map.h>

namespace toadlet{
namespace egg{

template<typename Class>
class BaseType{
public:
	virtual ~BaseType(){}
	virtual const String &getFullName() const=0;
	virtual Class *newInstance() const=0;
};

template<typename Class,typename BaseClass>
class TOADLET_API Type:public BaseType<BaseClass>{
public:
	Type(const char *fullName):mFullName(fullName){}

	const String &getFullName() const{return mFullName;}
	BaseClass *newInstance() const{return new Class();}

protected:
	String mFullName;
};

template<typename Class,typename BaseClass>
class TOADLET_API NonInstantiableType:public BaseType<BaseClass>{
public:
	NonInstantiableType(const char *fullName):mFullName(fullName){}

	const String &getFullName() const{return mFullName;}
	BaseClass *newInstance() const{return NULL;}

protected:
	String mFullName;
};

template<typename Class>
class TypeFactory{
public:
	TypeFactory(){}
	
	void registerType(BaseType<Class> *type){
		mTypes[type->getFullName()]=type;
	}
	
	void unregisterType(BaseType<Class> *type){
		mTypes.erase(type);
	}
	
	Class *newInstance(const String &fullName){
		typename Map<String,const BaseType<Class>*>::iterator i=mTypes.find(fullName);
		if(i==mTypes.end()){
			Error::unknown("unknown type: "+fullName);
			return NULL;
		}
		else{
			return i->second->newInstance();
		}
	}

protected:
	Map<String,const BaseType<Class>*> mTypes;
};

#define TOADLET_ALIGNED_NEW \
	inline void *operator new(size_t size){return TOADLET_ALIGNED_MALLOC(size,16);} \
	inline void operator delete(void *pointer){TOADLET_ALIGNED_FREE(pointer);}

}
}

#endif
