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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/handler/TMSHHandler.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace handler{

TMSHHandler::TMSHHandler(Engine *engine){
	mBufferManager=engine->getBufferManager();
	mMaterialManager=engine->getMaterialManager();
	mEngine=engine;
}

Resource::ptr TMSHHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	DataStream::ptr dataStream(new DataStream(stream));

	int signature=dataStream->readUInt32();
	if(signature!=SIGNATURE){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of TMSH format");
		return NULL;
	}

	int version=dataStream->readUInt32();
	if(version!=VERSION){
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("Not TMSH version:")+VERSION);
		return NULL;
	}

	Mesh::ptr mesh;
/*	Skeleton::ptr skeleton;
	Collection<Material::ptr> materials;
	Collection<TransformSequence::ptr> sequences;
	int i,j;
*/	while(stream->position()<stream->length()){
		int blockType=dataStream->readInt32();
		int blockSize=dataStream->readInt32();
		switch(blockType){
			case Block_MESH:
				mesh=readMesh(dataStream,blockSize);
			break;
/*			case Block_MATERIAL:
				materials.add(readMaterial(dataStream,blockSize));
			break;
			case Block_SKELETON:
				skeleton=readSkeleton(dataStream,blockSize);
			break;
			case Block_SEQUENCE:
				sequences.add(readSequence(dataStream,blockSize));
			break;
*/			default:
				stream->seek(stream->position()+blockSize);
			break;
		}
	}
/*
	TODO: Link mesh to materials & skeleton to sequences & mesh to skeleton
*/
	return mesh;
}

bool TMSHHandler::save(Mesh::ptr resource,Stream::ptr stream){
	DataStream::ptr dataStream(new DataStream(stream));

	dataStream->writeUInt32(SIGNATURE);

	dataStream->writeUInt32(VERSION);

	MemoryStream::ptr memoryStream(new MemoryStream());
	DataStream::ptr dataMemoryStream(new DataStream(shared_static_cast<Stream>(memoryStream)));

	writeMesh(dataMemoryStream,resource);
	dataStream->writeInt32(Block_MESH);
	dataStream->writeInt32(memoryStream->length());
	dataStream->write(memoryStream->getOriginalDataPointer(),memoryStream->length());
	memoryStream->reset();

	return true;
}

Mesh::ptr TMSHHandler::readMesh(DataStream *stream,int blockSize){
	int i;

	Mesh::ptr mesh(new Mesh());
	
	mesh->transform.read(stream);
	mesh->bound.read(stream);
	
	mesh->subMeshes.resize(stream->readUInt32());
	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
		mesh->subMeshes[i]=subMesh;
	
		subMesh->vertexData=readVertexData(stream);
		subMesh->indexData=readIndexData(stream);

		subMesh->materialName=stream->readNullTerminatedString();
		
		subMesh->name=stream->readNullTerminatedString();

		subMesh->hasOwnTransform=stream->readBool();
		subMesh->transform.read(stream);
		subMesh->bound.read(stream);
	}
	mesh->staticVertexData=readVertexData(stream);

	mesh->vertexBoneAssignments.resize(stream->readUInt32());
	for(i=0;i<mesh->vertexBoneAssignments.size();++i){
		Mesh::VertexBoneAssignmentList &list=mesh->vertexBoneAssignments[i];
		
		int numAssignments=stream->readUInt8();
		list.resize(numAssignments);
		stream->read((tbyte*)list.begin(),numAssignments*sizeof(Mesh::VertexBoneAssignment));
	}

	return mesh;
}

IndexData::ptr TMSHHandler::readIndexData(DataStream *stream){
	IndexData::Primitive primitive=(IndexData::Primitive)stream->readUInt32();
	if(primitive==0){
		return NULL;
	}

	IndexBuffer::ptr indexBuffer=readIndexBuffer(stream);
	int start=stream->readInt32();
	int cound=stream->readInt32();
	
	return IndexData::ptr(new IndexData(primitive,indexBuffer,start,cound));
}

IndexBuffer::ptr TMSHHandler::readIndexBuffer(DataStream *stream){
	int usage=stream->readUInt32();
	if(usage==0){
		return NULL;
	}

	int access=stream->readUInt32();
	IndexBuffer::IndexFormat format=(IndexBuffer::IndexFormat)stream->readUInt32();
	int size=stream->readUInt32();

	IndexBuffer::ptr buffer=mEngine->getBufferManager()->createIndexBuffer(usage,access,format,size);
	tbyte *data=buffer->lock(Buffer::Access_BIT_WRITE);
	stream->read(data,size*format);
	buffer->unlock();

	return buffer;
}

VertexData::ptr TMSHHandler::readVertexData(DataStream *stream){
	/// @todo: Really support multiple VertexBuffers
	int numVertexBuffers=stream->readUInt32();
	if(numVertexBuffers==0){
		return NULL;
	}

	VertexBuffer::ptr buffer=readVertexBuffer(stream);
	return VertexData::ptr(new VertexData(buffer));
}

VertexBuffer::ptr TMSHHandler::readVertexBuffer(DataStream *stream){
	int usage=stream->readUInt32();
	if(usage==0){
		return NULL;
	}

	int access=stream->readUInt32();
	VertexFormat::ptr format=readVertexFormat(stream);
	int size=stream->readUInt32();

	VertexBuffer::ptr buffer=mEngine->getBufferManager()->createVertexBuffer(usage,access,format,size);
	tbyte *data=buffer->lock(Buffer::Access_BIT_WRITE);
	stream->read(data,size*format->getVertexSize());
	buffer->unlock();

	return buffer;
}

VertexFormat::ptr TMSHHandler::readVertexFormat(DataStream *stream){
	int numElements=stream->readUInt32();
	if(numElements==0){
		return NULL;
	}

	VertexFormat::ptr vertexFormat=mEngine->getBufferManager()->createVertexFormat();

	int i;
	for(i=0;i<numElements;++i){
		int semantic=stream->readUInt32();
		int index=stream->readUInt32();
		int format=stream->readUInt32();

		vertexFormat->addElement(semantic,index,format);
	}

	return vertexFormat;
};


bool TMSHHandler::writeMesh(DataStream *stream,Mesh::ptr mesh){
	mesh->transform.write(stream);
	mesh->bound.write(stream);
	
	stream->writeUInt32(mesh->subMeshes.size());
	int i;
	for(i=0;i<mesh->subMeshes.size();++i){
		Mesh::SubMesh::ptr subMesh=mesh->subMeshes[i];

		writeVertexData(stream,subMesh->vertexData);
		writeIndexData(stream,subMesh->indexData);

		stream->writeNullTerminatedString(subMesh->materialName);

		stream->writeNullTerminatedString(subMesh->name);

		stream->writeBool(subMesh->hasOwnTransform);
		subMesh->transform.write(stream);
		subMesh->bound.write(stream);
	}
	writeVertexData(stream,mesh->staticVertexData);

	stream->writeUInt32(mesh->vertexBoneAssignments.size());
	for(i=0;i<mesh->vertexBoneAssignments.size();++i){
		Mesh::VertexBoneAssignmentList &list=mesh->vertexBoneAssignments[i];
		
		int numAssignments=list.size();
		stream->writeUInt8(numAssignments);
		stream->write((tbyte*)list.begin(),numAssignments*sizeof(Mesh::VertexBoneAssignment));
	}

	return true;
}

void TMSHHandler::writeIndexData(DataStream *stream,IndexData::ptr indexData){
	if(indexData==NULL){
		stream->writeUInt32(0);
		return;
	}

	stream->writeUInt32(indexData->primitive);
	writeIndexBuffer(stream,indexData->indexBuffer);
	stream->writeInt32(indexData->start);
	stream->writeInt32(indexData->count);
}

void TMSHHandler::writeIndexBuffer(DataStream *stream,IndexBuffer::ptr buffer){
	if(buffer==NULL){
		stream->writeUInt32(0);
		return;
	}

	stream->writeUInt32(buffer->getUsage());
	stream->writeUInt32(buffer->getAccess());
	stream->writeUInt32(buffer->getIndexFormat());
	stream->writeUInt32(buffer->getSize());

	uint8 *data=buffer->lock(Buffer::Access_BIT_READ);
	stream->write(data,buffer->getSize()*buffer->getIndexFormat());
	buffer->unlock();
}

void TMSHHandler::writeVertexData(DataStream *stream,VertexData::ptr vertexData){
	/// @todo: Support multiple VertexBuffers
	if(vertexData==NULL){
		stream->writeUInt32(0);
		return;
	}

	stream->writeUInt32(1);
	writeVertexBuffer(stream,vertexData->vertexBuffers[0]);
}

void TMSHHandler::writeVertexBuffer(DataStream *stream,VertexBuffer::ptr buffer){
	if(buffer==NULL){
		stream->writeUInt32(0);
		return;
	}

	stream->writeUInt32(buffer->getUsage());
	stream->writeUInt32(buffer->getAccess());
	writeVertexFormat(stream,buffer->getVertexFormat());
	stream->writeUInt32(buffer->getSize());

	uint8 *data=buffer->lock(Buffer::Access_BIT_READ);
	stream->write(data,buffer->getSize()*buffer->getVertexFormat()->getVertexSize());
	buffer->unlock();
}

void TMSHHandler::writeVertexFormat(DataStream *stream,VertexFormat::ptr vertexFormat){
	if(vertexFormat==NULL){
		stream->writeUInt32(0);
	}

	int numElements=vertexFormat->getNumElements();
	stream->writeUInt32(numElements);

	int i;
	for(i=0;i<numElements;++i){
		stream->writeUInt32(vertexFormat->getSemantic(i));
		stream->writeUInt32(vertexFormat->getIndex(i));
		stream->writeUInt32(vertexFormat->getFormat(i));
	}
};

}
}
}
