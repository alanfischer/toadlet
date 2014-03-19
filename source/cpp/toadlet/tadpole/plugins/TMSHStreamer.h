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

#ifndef TOADLET_TADPOLE_TMSHSTREAMER_H
#define TOADLET_TADPOLE_TMSHSTREAMER_H

#include <toadlet/egg/io/DataStream.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/BaseResourceStreamer.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Mesh.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API TMSHStreamer:public BaseResourceStreamer{
public:
	TOADLET_OBJECT(TMSHStreamer);

	const static int VERSION=0x10001;
	const static int TMSH=1414353736; // "TMSH"

	TMSHStreamer(Engine *engine);

	bool load(Stream::ptr stream,ResourceData *data,ResourceRequest *request);
	bool save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ResourceRequest *request);

protected:
	class MaterialRequest:public Object,public ResourceRequest{
	public:
		TOADLET_IOBJECT(MaterialRequest);

		MaterialRequest(Engine *engine,Mesh::ptr mesh,ResourceRequest *request):mEngine(engine),mMesh(mesh),mRequest(request),mIndex(0){}

		void request();

		void resourceReady(Resource *resource);
		void resourceException(const Exception &ex);
		void resourceProgress(float progress){}

	protected:
		Engine *mEngine;
		Mesh::ptr mMesh;
		ResourceRequest::ptr mRequest;
		int mIndex;
	};

	Mesh::ptr readMesh(DataStream *stream,int blockSize);
	void writeMesh(DataStream *stream,Mesh::ptr mesh);

	Bound::ptr readBound(DataStream *stream);
	void writeBound(DataStream *stream,Bound::ptr bound);

	Transform::ptr readTransform(DataStream *stream);
	void writeTransform(DataStream *stream,Transform::ptr transform);

	IndexData::ptr readIndexData(DataStream *stream);
	IndexBuffer::ptr readIndexBuffer(DataStream *stream);
	VertexData::ptr readVertexData(DataStream *stream);
	VertexBuffer::ptr readVertexBuffer(DataStream *stream);
	VertexFormat::ptr readVertexFormat(DataStream *stream);
	void writeIndexData(DataStream *stream,IndexData::ptr indexData);
	void writeIndexBuffer(DataStream *stream,IndexBuffer::ptr buffer);
	void writeVertexData(DataStream *stream,VertexData::ptr vertexData);
	void writeVertexBuffer(DataStream *stream,VertexBuffer::ptr buffer);
	void writeVertexFormat(DataStream *stream,VertexFormat::ptr vertexFormat);

	Material::ptr readMaterial(DataStream *stream,int blockSize);
	void writeMaterial(DataStream *stream,Material::ptr material);

	Skeleton::ptr readSkeleton(DataStream *stream,int blockSize);
	void writeSkeleton(DataStream *stream,Skeleton::ptr skeleton);

	Sequence::ptr readSequence(DataStream *stream,int blockSize);
	void writeSequence(DataStream *stream,Sequence::ptr sequence);

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

#endif

