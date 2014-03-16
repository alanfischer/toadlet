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

#ifndef TOADLET_EGG_IO_BASEARCHIVE_H
#define TOADLET_EGG_IO_BASEARCHIVE_H

#include <toadlet/egg/Object.h>
#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/io/Archive.h>

namespace toadlet{
namespace egg{
namespace io{

class BaseArchive:public BaseResource,public Archive{
public:
	TOADLET_RESOURCE(BaseArchive,Archive);

	bool openStream(const String &name,StreamRequest *request){
		Stream::ptr result;
		Exception exception;
		TOADLET_TRY
			result=openStream(name);
		TOADLET_CATCH(Exception ex){exception=ex;}
		if(result!=NULL){
			request->streamReady(result);
			return true;
		}
		else{
			request->streamException(exception);
			return false;
		}
	}

	bool openResource(const String &name,ResourceRequest *request){
		Resource::ptr result;
		Exception exception;
		TOADLET_TRY
			result=openResource(name);
		TOADLET_CATCH(Exception ex){exception=ex;}
		if(result!=NULL){
			request->resourceReady(result);
			return true;
		}
		else{
			request->resourceException(exception);
			return false;
		}
	}

	virtual Stream::ptr openStream(const String &name){return NULL;}
	virtual Resource::ptr openResource(const String &name){return NULL;}
};

}
}
}

#endif
