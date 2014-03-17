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

#ifndef TOADLET_TADPOLE_BASERESOURCESTREAMER_H
#define TOADLET_TADPOLE_BASERESOURCESTREAMER_H

#include <toadlet/egg/Object.h>
#include <toadlet/tadpole/ResourceStreamer.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API BaseResourceStreamer:public Object,public ResourceStreamer{
public:
	TOADLET_IOBJECT(BaseResourceStreamer);

	BaseResourceStreamer(){}

	bool load(Stream::ptr stream,ResourceData *data,ResourceRequest *request){
		Resource::ptr result;
		Exception exception;
		TOADLET_TRY
			result=load(stream,data);
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

	bool save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ResourceRequest *request){
		bool result=false;
		Exception exception;
		TOADLET_TRY
			result=save(stream,resource,data);
		TOADLET_CATCH(Exception ex){exception=ex;}
		if(result){
			request->resourceReady(resource);
			return true;
		}
		else{
			request->resourceException(exception);
			return false;
		}
	}

	virtual Resource::ptr load(Stream::ptr stream,ResourceData *data){return NULL;}
	virtual bool save(Stream::ptr stream,Resource::ptr resource,ResourceData *data){return false;}
};

}
}

#endif

