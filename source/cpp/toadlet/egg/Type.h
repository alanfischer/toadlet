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
class Type{
public:
	virtual ~Type(){}
	virtual const String &getFullName() const=0;
	virtual bool instanceOf(const String &fullName) const=0;
	virtual Class *newInstance() const=0;
};

template<typename Class,typename SuperClass>
class TOADLET_API InstantiableType:public Type<SuperClass>{
public:
	InstantiableType(const char *fullName,Type<SuperClass> *superType=NULL):mFullName(fullName),mSuperType(superType){}
	const String &getFullName() const{return mFullName;}
	bool instanceOf(const String &fullName) const{return mFullName==fullName || (mSuperType!=NULL && mSuperType->instanceOf(fullName));}
	SuperClass *newInstance() const{return new Class();}

protected:
	String mFullName;
	Type<SuperClass> *mSuperType;
};

template<typename Class,typename SuperClass>
class TOADLET_API NonInstantiableType:public Type<SuperClass>{
public:
	NonInstantiableType(const char *fullName,Type<SuperClass> *superType=NULL):mFullName(fullName),mSuperType(superType){}
	const String &getFullName() const{return mFullName;}
	bool instanceOf(const String &fullName) const{return mFullName==fullName || (mSuperType!=NULL && mSuperType->instanceOf(fullName));}
	SuperClass *newInstance() const{return NULL;}

protected:
	String mFullName;
	Type<SuperClass> *mSuperType;
};

template<typename Class>
class TypeFactory{
public:
	TypeFactory(){}
	
	void registerType(Type<Class> *type){
		mTypes[type->getFullName()]=type;
	}
	
	void unregisterType(Type<Class> *type){
		mTypes.erase(type);
	}
	
	Class *newInstance(const String &fullName){
		typename Map<String,const Type<Class>*>::iterator i=mTypes.find(fullName);
		if(i==mTypes.end()){
			Error::unknown("unknown type: "+fullName);
			return NULL;
		}
		else{
			return i->second->newInstance();
		}
	}

protected:
	Map<String,const Type<Class>*> mTypes;
};

}
}

#endif
