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

#ifndef TOADLET_TADPOLE_HANDLER_XMATHANDLER_H
#define TOADLET_TADPOLE_HANDLER_XMATHANDLER_H

#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/ResourceHandler.h>
#include <toadlet/tadpole/ResourceManager.h>

typedef struct mxml_node_s mxml_node_t;

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API XMATHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(XMATHandler,ResourceHandler);

	XMATHandler(ResourceManager *textureManager);

	egg::Resource::ptr load(egg::io::InputStream::ptr in,const ResourceHandlerData *handlerData);
	bool save(Material::ptr resource,egg::io::OutputStream::ptr out);

protected:
	Material::ptr loadMaterial(mxml_node_t *root,int version);
	bool saveMaterial(mxml_node_t *root,Material::ptr jmaterial,int version);

	ResourceManager *mTextureManager;
};

}
}
}

#endif
