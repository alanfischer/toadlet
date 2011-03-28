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

#ifndef TOADLET_TADPOLE_HANDLER_TMSHHANDLER_H
#define TOADLET_TADPOLE_HANDLER_TMSHHANDLER_H

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

class TOADLET_API TMSHHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(TMSHHandler);

	const static int VERSION=1;
	const static int TMSH=1414353736; // "TMSH"

	TMSHHandler(Engine *engine);

	egg::Resource::ptr load(egg::io::Stream::ptr stream,const ResourceHandlerData *handlerData);
	bool save(Mesh::ptr mesh,egg::io::Stream::ptr stream);

protected:
	Mesh::ptr readMesh(egg::io::DataStream *stream,int blockSize);
	peeper::IndexData::ptr readIndexData(egg::io::DataStream *stream);
	peeper::IndexBuffer::ptr readIndexBuffer(egg::io::DataStream *stream);
	peeper::VertexData::ptr readVertexData(egg::io::DataStream *stream);
	peeper::VertexBuffer::ptr readVertexBuffer(egg::io::DataStream *stream);
	peeper::VertexFormat::ptr readVertexFormat(egg::io::DataStream *stream);
	void writeMesh(egg::io::DataStream *stream,Mesh::ptr mesh);
	void writeIndexData(egg::io::DataStream *stream,peeper::IndexData::ptr indexData);
	void writeIndexBuffer(egg::io::DataStream *stream,peeper::IndexBuffer::ptr buffer);
	void writeVertexData(egg::io::DataStream *stream,peeper::VertexData::ptr vertexData);
	void writeVertexBuffer(egg::io::DataStream *stream,peeper::VertexBuffer::ptr buffer);
	void writeVertexFormat(egg::io::DataStream *stream,peeper::VertexFormat::ptr vertexFormat);

	Material::ptr readMaterial(egg::io::DataStream *stream,int blockSize);
	void writeMaterial(egg::io::DataStream *stream,Material::ptr material);

	// Blocks for the tmsh format
	enum Block{
		Block_MESH=		1,
		Block_MATERIAL=	2,
		Block_SKELETON= 3,
		Block_SEQUENCE=	4,
	};

	BufferManager *mBufferManager;
	MaterialManager *mMaterialManager;
	Engine *mEngine;
};

}
}
}

#endif

