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

#ifndef TOADLET_EGG_BASERESOURCE_H
#define TOADLET_EGG_BASERESOURCE_H

#include <toadlet/egg/Assert.h>
#include <toadlet/egg/Object.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/egg/ResourceDestroyedListener.h>

namespace toadlet{
namespace egg{

class BaseResource:public Object,public Resource{
public:
	BaseResource():Object(),mListener(NULL),mName(),mUniqueHandle(0){}
	virtual ~BaseResource(){}

	virtual void destroy(){
		if(mListener!=NULL){
			mListener->resourceDestroyed(resourceThis());
			mListener=NULL;
		}
	}

	virtual Resource *resourceThis(){return this;}

	void setDestroyedListener(ResourceDestroyedListener *listener){mListener=listener;}

	void setName(const String &name){mName=name;}
	const String &getName() const{return mName;}

	void internal_setUniqueHandle(int handle){mUniqueHandle=handle;}
	int getUniqueHandle() const{return mUniqueHandle;}

protected:
	ResourceDestroyedListener *mListener;
	String mName;
	int mUniqueHandle;
};

}
}

// Help to work around an issue with c++ multiple inheritance issues
#define TOADLET_RESOURCE(Class,BaseClass)\
	TOADLET_IOBJECT(Class); \
	toadlet::egg::Resource *resourceThis(){return (BaseClass*)this;} \
	void setDestroyedListener(toadlet::egg::ResourceDestroyedListener *listener){toadlet::egg::BaseResource::setDestroyedListener(listener);} \
	void setName(const toadlet::egg::String &name){toadlet::egg::BaseResource::setName(name);}\
	const toadlet::egg::String &getName() const{return toadlet::egg::BaseResource::getName();}\
	void internal_setUniqueHandle(int handle){toadlet::egg::BaseResource::internal_setUniqueHandle(handle);}\
	int getUniqueHandle() const{return toadlet::egg::BaseResource::getUniqueHandle();}

#endif
