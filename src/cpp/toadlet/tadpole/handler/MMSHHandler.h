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

#ifndef TOADLET_TADPOLE_HANDLER_MMSHHANDLER_H
#define TOADLET_TADPOLE_HANDLER_MMSHHANDLER_H

#include <toadlet/egg/io/DataStream.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/ResourceHandler.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API MMSHHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(MMSHHandler);

	MMSHHandler(Engine *engine);

	egg::Resource::ptr load(egg::io::Stream::ptr in,const ResourceHandlerData *handlerData);

protected:
	Mesh::ptr loadMeshVersion3(egg::io::DataStream::ptr stream);
	Mesh::ptr loadMeshVersion4Up(egg::io::DataStream::ptr stream,int version);
	bool saveMeshVersion4Up(egg::io::DataStream::ptr stream,int version);

	// Blocks for the mmsh format
	enum{
		MESH_BLOCK=1,
		SKELETON_BLOCK=2,
		ANIMATION_BLOCK=3,
		MATERIAL_BLOCK=4,
	};

	// VertexTypes for the mmsh v3 format
	enum{
		VT_POSITION=1<<0,
		VT_NORMAL=1<<1,
		VT_COLOR=1<<2,
		VT_BONE=1<<3,
		VT_TEXCOORD1=1<<8,
	};

	BufferManager *mBufferManager;
	MaterialManager *mMaterialManager;
	Engine *mEngine;
};

}
}
}

#endif

