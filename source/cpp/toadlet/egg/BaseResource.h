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

#include <toadlet/egg/PointerCounter.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/egg/ResourceFullyReleasedListener.h>

namespace toadlet{
namespace egg{

class BaseResource:public Resource{
public:
	TOADLET_SHARED_POINTERS(BaseResource);

	BaseResource():mReference(0),mListener(NULL),mName(),mUniqueHandle(0){
//mPointerCounter=new PointerCounter(0);
	}
	virtual ~BaseResource(){
//mListener->resourceFullyReleased(this);
	}

	virtual void destroy()=0;

	void retain(){++mReference;}
	void release(){if(--mReference==0){mListener!=NULL?mListener->resourceFullyReleased(this):destroy();}}
PointerCounter *pointerCounter(){return mPointerCounter;}

	void setFullyReleasedListener(ResourceFullyReleasedListener *listener){mListener=listener;}

	void setName(const String &name){mName=name;}
	const String &getName() const{return mName;}

	void internal_setUniqueHandle(int handle){mUniqueHandle=handle;}
	int getUniqueHandle() const{return mUniqueHandle;}

//	int getReferenceCount(){return mReference;}

protected:
	int mReference;
PointerCounter *mPointerCounter;
	ResourceFullyReleasedListener *mListener;
	String mName;
	int mUniqueHandle;
};

}
}

// Help to work around an annoying issue with c++ multiple inheritance issues
//  Since I can't just call BaseResource::release(), since it will use a different base pointer
//  This issue doesn't exist in other languages (Java)
#define TOADLET_BASERESOURCE_PASSTHROUGH(BaseClass)\
	void retain(){toadlet::egg::BaseResource::retain();} \
	void release(){if(--mReference==0){mListener!=NULL?mListener->resourceFullyReleased((BaseClass*)this):destroy();}} \
toadlet::egg::PointerCounter *pointerCounter(){return toadlet::egg::BaseResource::pointerCounter();} \
	void setFullyReleasedListener(toadlet::egg::ResourceFullyReleasedListener *listener){toadlet::egg::BaseResource::setFullyReleasedListener(listener);} \
	void setName(const toadlet::egg::String &name){toadlet::egg::BaseResource::setName(name);}\
	const toadlet::egg::String &getName() const{return toadlet::egg::BaseResource::getName();}\
	void internal_setUniqueHandle(int handle){toadlet::egg::BaseResource::internal_setUniqueHandle(handle);}\
	int getUniqueHandle() const{return toadlet::egg::BaseResource::getUniqueHandle();}

#endif
