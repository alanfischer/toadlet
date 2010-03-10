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

#ifndef TOADLET_TADPOLE_BSP_BSP30HANDLER_H
#define TOADLET_TADPOLE_BSP_BSP30HANDLER_H

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ResourceHandler.h>
#include <toadlet/tadpole/bsp/BSP30Map.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class BSP30Handler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(BSP30Handler);

	BSP30Handler(Engine *engine);
	virtual ~BSP30Handler();

	virtual egg::Resource::ptr load(egg::io::Stream::ptr stream,const ResourceHandlerData *handlerData);

protected:
	void readLump(egg::io::Stream *stream,blump *lump,void **data,int size,int *count);
	void parseVisibility(BSP30Map *map);
	void parseEntities(BSP30Map *map);
	void parseWADs(BSP30Map *map);
	void parseTextures(BSP30Map *map);

	Engine *mEngine;
};

}
}
}

#endif
