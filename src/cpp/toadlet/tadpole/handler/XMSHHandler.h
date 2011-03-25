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

#ifndef TOADLET_TADPOLE_HANDLER_XMSHHANDLER_H
#define TOADLET_TADPOLE_HANDLER_XMSHHANDLER_H

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>

typedef struct mxml_node_s mxml_node_t;

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API XMSHHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(XMSHHandler);

	XMSHHandler(Engine *engine);

	egg::Resource::ptr load(egg::io::Stream::ptr stream,const ResourceHandlerData *handlerData);

	bool save(Mesh::ptr resource,egg::io::Stream::ptr stream,ProgressListener *listener=NULL);

protected:
	Mesh::ptr loadMeshVersion1(mxml_node_t *root);
	Mesh::ptr loadMeshVersion2Up(mxml_node_t *root,int version);
	bool saveMeshVersion1(mxml_node_t *root,Mesh::ptr mesh,ProgressListener *listener);
	bool saveMeshVersion2Up(mxml_node_t *root,Mesh::ptr mesh,int version,ProgressListener *listener);

	Engine *mEngine;
};

}
}
}

#endif
