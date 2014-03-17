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

#ifndef TOADLET_TADPOLE_BASERESOURCECREATOR_H
#define TOADLET_TADPOLE_BASERESOURCECREATOR_H

#include <toadlet/egg/Object.h>
#include <toadlet/tadpole/ResourceCreator.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API BaseResourceCreator:public Object,public ResourceCreator{
public:
	TOADLET_IOBJECT(BaseResourceCreator);

	BaseResourceCreator(){}

	bool create(const String &name,ResourceData *data,ResourceRequest *request){
		Resource::ptr result;
		Exception exception;
		TOADLET_TRY
			result=create(name,data);
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

	virtual Resource::ptr create(const String &name,ResourceData *data)=0;
};

}
}

#endif

