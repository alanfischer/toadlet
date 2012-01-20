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
#include <toadlet/tadpole/ResourceStreamer.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API TMSHHandler:public ResourceStreamer{
public:
	TOADLET_SHARED_POINTERS(TMSHHandler);

	const static int VERSION=0x10000;
	const static int TMSH=1414353736; // "TMSH"

	TMSHHandler(Engine *engine);

	Resource::ptr load(Stream::ptr stream,ResourceData *data,ProgressListener *listener);
	bool save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ProgressListener *listener);

protected:
	Mesh::ptr readMesh(DataStream *stream,int blockSize);
	IndexData::ptr readIndexData(DataStream *stream);
	IndexBuffer::ptr readIndexBuffer(DataStream *stream);
	VertexData::ptr readVertexData(DataStream *stream);
	VertexBuffer::ptr readVertexBuffer(DataStream *stream);
	VertexFormat::ptr readVertexFormat(DataStream *stream);
	void writeMesh(DataStream *stream,Mesh::ptr mesh);
	void writeIndexData(DataStream *stream,IndexData::ptr indexData);
	void writeIndexBuffer(DataStream *stream,IndexBuffer::ptr buffer);
	void writeVertexData(DataStream *stream,VertexData::ptr vertexData);
	void writeVertexBuffer(DataStream *stream,VertexBuffer::ptr buffer);
	void writeVertexFormat(DataStream *stream,VertexFormat::ptr vertexFormat);

	Material::ptr readMaterial(DataStream *stream,int blockSize);
	void writeMaterial(DataStream *stream,Material::ptr material);

	Skeleton::ptr readSkeleton(DataStream *stream,int blockSize);
	void writeSkeleton(DataStream *stream,Skeleton::ptr skeleton);

	TransformSequence::ptr readSequence(DataStream *stream,int blockSize);
	void writeSequence(DataStream *stream,TransformSequence::ptr sequence);

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

