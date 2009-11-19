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

namespace toadlet{
namespace egg{

template<typename Class>
class BaseType{
public:
	virtual ~BaseType(){}
	virtual const char *getFullName() const=0;
	virtual Class *newInstance() const=0;
};

template<typename Class,typename BaseClass>
class TOADLET_API Type:public BaseType<BaseClass>{
public:
	Type(const char *fullName):mFullName(fullName){}

	const char *getFullName() const{return mFullName;}
	BaseClass *newInstance() const{return new Class();}

protected:
	String mFullName;
};

}
}

#endif
