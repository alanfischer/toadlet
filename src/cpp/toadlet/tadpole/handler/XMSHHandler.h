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

#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/mesh/Mesh.h>

typedef struct mxml_node_s mxml_node_t;

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API XMSHHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(XMSHHandler);

	XMSHHandler(BufferManager *bufferManager,ResourceManager *materialManager,TextureManager *textureManager);

	egg::Resource::ptr load(egg::io::InputStream::ptr in,const ResourceHandlerData *handlerData);

	bool save(mesh::Mesh::ptr resource,egg::io::OutputStream::ptr out);

protected:
	mesh::Mesh::ptr loadMeshVersion1(mxml_node_t *root);
	mesh::Mesh::ptr loadMeshVersion2Up(mxml_node_t *root,int version);
	bool saveMeshVersion1(mxml_node_t *root,mesh::Mesh::ptr mesh);
	bool saveMeshVersion2Up(mxml_node_t *root,mesh::Mesh::ptr mesh,int version);

	BufferManager *mBufferManager;
	ResourceManager *mMaterialManager;
	TextureManager *mTextureManager;
};

}
}
}

#endif
