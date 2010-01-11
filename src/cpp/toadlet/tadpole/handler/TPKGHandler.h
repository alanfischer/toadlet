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

#ifndef TOADLET_TADPOLE_HANDLER_TPKGHANDLER_H
#define TOADLET_TADPOLE_HANDLER_TPKGHANDLER_H

#include <toadlet/egg/io/TPKGArchive.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API TPKGHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(TPKGHandler);

	TPKGHandler(){}

	egg::Resource::ptr load(egg::io::Stream::ptr stream,const ResourceHandlerData *handlerData){
		egg::io::TPKGArchive::ptr archive(new egg::io::TPKGArchive());
		bool result=archive->open(stream);
		if(result){
			return egg::shared_static_cast<egg::io::Archive>(archive);
		}
		else{
			return NULL;
		}
	}
};

}
}
}

#endif

