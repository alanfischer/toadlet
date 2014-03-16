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

#include <toadlet/egg/Extents.h>
#include <toadlet/egg/io/MemoryStream.h>
#include "TMSHStreamer.h"

namespace toadlet{
namespace tadpole{

TMSHStreamer::TMSHStreamer(Engine *engine){
	mBufferManager=engine->getBufferManager();
	mMaterialManager=engine->getMaterialManager();
	mEngine=engine;
}

bool TMSHStreamer::load(Stream::ptr stream,ResourceData *data,ResourceRequest *request){
	int i,j;

	DataStream::ptr dataStream=new DataStream(stream);

	int id=dataStream->readUInt32();
	if(id!=TMSH){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Not of TMSH format");
		return false;
	}

	int version=dataStream->readUInt32();
	if(version!=VERSION){
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("Not TMSH version:")+VERSION);
		return false;
	}

	Mesh::ptr mesh;
	Collection<Material::ptr> materials;
	Skeleton::ptr skeleton;
	Collection<Sequence::ptr> sequences;
	while(stream->position()<stream->length()){
		int blockType=dataStream->readInt32();
		int blockSize=dataStream->readInt32();
		switch(blockType){
			case Block_MESH:
				mesh=readMesh(dataStream,blockSize);
			break;
			case Block_MATERIAL:
				materials.add(readMaterial(dataStream,blockSize));
			break;
			case Block_SKELETON:
				skeleton=readSkeleton(dataStream,blockSize);
			break;
			case Block_SEQUENCE:
				sequences.add(readSequence(dataStream,blockSize));
			break;
			default:
				stream->seek(stream->position()+blockSize);
			break;
		}
	}

	if(skeleton!=NULL){
		for(i=0;i<sequences.size();++i){
			skeleton->addSequence(sequences[i]);
		}
		mesh->setSkeleton(skeleton);
	}

	j=0;
	for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh *subMesh=mesh->getSubMesh(i);
		if(subMesh->materialName.length()==0 && j<materials.size()){
			subMesh->material=materials[j++];
		}
	}
	
	MaterialRequest::ptr materialRequest=new MaterialRequest(mEngine->getMaterialManager(),mesh,request);
	materialRequest->request();

	return true;
}

bool TMSHStreamer::save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ResourceRequest *request){
	Mesh::ptr mesh=shared_static_cast<Mesh>(resource);
	if(mesh==NULL){
		return false;
	}

	int i;
	DataStream::ptr dataStream=new DataStream(stream);

	dataStream->writeUInt32(TMSH);

	dataStream->writeUInt32(VERSION);

	MemoryStream::ptr memoryStream=new MemoryStream();
	DataStream::ptr dataMemoryStream=new DataStream(memoryStream);

	writeMesh(dataMemoryStream,mesh);
	dataStream->writeInt32(Block_MESH);
	dataStream->writeInt32(memoryStream->length());
	dataStream->write(memoryStream->getOriginalDataPointer(),memoryStream->length());
	memoryStream->reset();

	for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh *subMesh=mesh->getSubMesh(i);
		if(subMesh->materialName.length()==0){
			writeMaterial(dataMemoryStream,subMesh->material);
			dataStream->writeInt32(Block_MATERIAL);
			dataStream->writeInt32(memoryStream->length());
			dataStream->write(memoryStream->getOriginalDataPointer(),memoryStream->length());
			memoryStream->reset();
		}
	}

	if(mesh->getSkeleton()!=NULL){
		Skeleton::ptr skeleton=mesh->getSkeleton();
		writeSkeleton(dataMemoryStream,skeleton);
		dataStream->writeInt32(Block_SKELETON);
		dataStream->writeInt32(memoryStream->length());
		dataStream->write(memoryStream->getOriginalDataPointer(),memoryStream->length());
		memoryStream->reset();

		for(i=0;i<skeleton->getNumSequences();++i){
			Sequence::ptr sequence=skeleton->getSequence(i);
			writeSequence(dataMemoryStream,sequence);
			dataStream->writeInt32(Block_SEQUENCE);
			dataStream->writeInt32(memoryStream->length());
			dataStream->write(memoryStream->getOriginalDataPointer(),memoryStream->length());
			memoryStream->reset();
		}
	}

	request->resourceReady(resource);

	return true;
}

Mesh::ptr TMSHStreamer::readMesh(DataStream *stream,int blockSize){
	int i;
	Mesh::ptr mesh=new Mesh();
	
	mesh->setTransform(readTransform(stream));

	mesh->setBound(readBound(stream));

	int numSubMeshes=stream->readUInt32();
	if(numSubMeshes>Extents::MAX_SHORT){
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("too many submeshes:")+numSubMeshes);
		return NULL;
	}
	for(i=0;i<numSubMeshes;++i){
		Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();

		subMesh->vertexData=readVertexData(stream);
		subMesh->indexData=readIndexData(stream);

		subMesh->materialName=stream->readNullTerminatedString();
		
		subMesh->setName(stream->readNullTerminatedString());

		stream->readBool(); // Depricated

		subMesh->transform=readTransform(stream);
		subMesh->bound=readBound(stream);

		mesh->addSubMesh(subMesh);
	}
	mesh->setStaticVertexData(readVertexData(stream));

	Collection<Mesh::VertexBoneAssignmentList> vbas;
	vbas.resize(stream->readUInt32());
	for(i=0;i<vbas.size();++i){
		Mesh::VertexBoneAssignmentList &list=vbas[i];
		
		int numAssignments=stream->readUInt8();
		list.resize(numAssignments);
		stream->read((tbyte*)&list[0],numAssignments*sizeof(Mesh::VertexBoneAssignment));
	}
	mesh->setVertexBoneAssignments(vbas);

	return mesh;
}

void TMSHStreamer::writeMesh(DataStream *stream,Mesh::ptr mesh){
	writeTransform(stream,mesh->getTransform());

	writeBound(stream,mesh->getBound());
	
	stream->writeUInt32(mesh->getNumSubMeshes());
	int i;
	for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh::ptr subMesh=mesh->getSubMesh(i);

		writeVertexData(stream,subMesh->vertexData);
		writeIndexData(stream,subMesh->indexData);

		stream->writeNullTerminatedString(subMesh->materialName);

		stream->writeNullTerminatedString(subMesh->getName());

		stream->writeBool(false); // Depricated

		writeTransform(stream,subMesh->transform);
		writeBound(stream,subMesh->bound);
	}
	writeVertexData(stream,mesh->getStaticVertexData());

	const Collection<Mesh::VertexBoneAssignmentList> &vbas=mesh->getVertexBoneAssignments();
	stream->writeUInt32(vbas.size());
	for(i=0;i<vbas.size();++i){
		const Mesh::VertexBoneAssignmentList &list=vbas[i];
		int numAssignments=list.size();
		stream->writeUInt8(numAssignments);
		stream->write((tbyte*)&list[0],numAssignments*sizeof(Mesh::VertexBoneAssignment));
	}
}

Bound::ptr TMSHStreamer::readBound(DataStream *stream){
	Bound::Type type=(Bound::Type)stream->readInt32();
	if(type==Bound::Type_EMPTY){
		return NULL;
	}

	AABox box;
	Sphere sphere;

	stream->readVector3(box.mins);
	stream->readVector3(box.maxs);
	stream->readVector3(sphere.origin);
	sphere.radius=stream->readFloat();

	Bound::ptr bound;
	if(type==Bound::Type_AABOX){
		bound=new Bound(box);
	}
	else if(type==Bound::Type_SPHERE){
		bound=new Bound(sphere);
	}
	else{
		bound=new Bound(type);
	}
	return bound;
}

void TMSHStreamer::writeBound(DataStream *stream,Bound::ptr bound){
	if(bound==NULL || bound->getType()==Bound::Type_EMPTY){
		stream->writeInt32(Bound::Type_EMPTY);
		return;
	}

	stream->writeInt32(bound->getType());
	stream->writeVector3(bound->getAABox().mins);
	stream->writeVector3(bound->getAABox().maxs);
	stream->writeVector3(bound->getSphere().origin);
	stream->writeFloat(bound->getSphere().radius);
}

Transform::ptr TMSHStreamer::readTransform(DataStream *stream){
	if(stream->readBool()==false){
		return NULL;
	}

	Vector3 translate;
	Vector3 scale;
	Quaternion rotate;

	stream->readVector3(translate);
	stream->readVector3(scale);
	stream->readQuaternion(rotate);

	return new Transform(translate,scale,rotate);
}

void TMSHStreamer::writeTransform(DataStream *stream,Transform::ptr transform){
	if(transform==NULL){
		stream->writeBool(false);
		return;
	}

	stream->writeBool(true);
	stream->writeVector3(transform->getTranslate());
	stream->writeVector3(transform->getScale());
	stream->writeQuaternion(transform->getRotate());
}

IndexData::ptr TMSHStreamer::readIndexData(DataStream *stream){
	IndexData::Primitive primitive=(IndexData::Primitive)stream->readUInt32();
	if(primitive==0){
		return NULL;
	}

	IndexBuffer::ptr buffer=readIndexBuffer(stream);
	int start=stream->readInt32();
	int cound=stream->readInt32();
	
	return buffer!=NULL?new IndexData(primitive,buffer,start,cound):NULL;
}

IndexBuffer::ptr TMSHStreamer::readIndexBuffer(DataStream *stream){
	int usage=stream->readUInt32();
	if(usage==0){
		return NULL;
	}

	int access=stream->readUInt32();
	IndexBuffer::IndexFormat format=(IndexBuffer::IndexFormat)stream->readUInt32();
	int size=stream->readUInt32();

	IndexBuffer::ptr buffer=mEngine->getBufferManager()->createIndexBuffer(usage,access,format,size);
	if(buffer!=NULL){
		tbyte *data=buffer->lock(Buffer::Access_BIT_WRITE);
		stream->read(data,size*format);
		buffer->unlock();
	}
	else{
		stream->seek(stream->position()+size*format);
	}

	return buffer;
}

VertexData::ptr TMSHStreamer::readVertexData(DataStream *stream){
	/// @todo: Really support multiple VertexBuffers
	int numVertexBuffers=stream->readUInt32();
	if(numVertexBuffers==0){
		return NULL;
	}

	VertexBuffer::ptr buffer=readVertexBuffer(stream);
	return buffer!=NULL?new VertexData(buffer):NULL;
}

VertexBuffer::ptr TMSHStreamer::readVertexBuffer(DataStream *stream){
	int usage=stream->readUInt32();
	if(usage==0){
		return NULL;
	}

	int access=stream->readUInt32();
	VertexFormat::ptr format=readVertexFormat(stream);
	int size=stream->readUInt32();

	VertexBuffer::ptr buffer=mEngine->getBufferManager()->createVertexBuffer(usage,access,format,size);
	if(buffer!=NULL){
		tbyte *data=buffer->lock(Buffer::Access_BIT_WRITE);
		stream->read(data,size*format->getVertexSize());
		buffer->unlock();
	}
	else{
		stream->seek(stream->position()+size*format->getVertexSize());
	}

	return buffer;
}

VertexFormat::ptr TMSHStreamer::readVertexFormat(DataStream *stream){
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

void TMSHStreamer::writeIndexData(DataStream *stream,IndexData::ptr indexData){
	if(indexData==NULL){
		stream->writeUInt32(0);
		return;
	}

	stream->writeUInt32(indexData->primitive);
	writeIndexBuffer(stream,indexData->indexBuffer);
	stream->writeInt32(indexData->start);
	stream->writeInt32(indexData->count);
}

void TMSHStreamer::writeIndexBuffer(DataStream *stream,IndexBuffer::ptr buffer){
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

void TMSHStreamer::writeVertexData(DataStream *stream,VertexData::ptr vertexData){
	/// @todo: Support multiple VertexBuffers
	if(vertexData==NULL){
		stream->writeUInt32(0);
		return;
	}

	stream->writeUInt32(1);
	writeVertexBuffer(stream,vertexData->vertexBuffers[0]);
}

void TMSHStreamer::writeVertexBuffer(DataStream *stream,VertexBuffer::ptr buffer){
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

void TMSHStreamer::writeVertexFormat(DataStream *stream,VertexFormat::ptr vertexFormat){
	if(vertexFormat==NULL){
		stream->writeUInt32(0);
	}

	int numElements=vertexFormat->getNumElements();
	stream->writeUInt32(numElements);

	int i;
	for(i=0;i<numElements;++i){
		stream->writeUInt32(vertexFormat->getElementSemantic(i));
		stream->writeUInt32(vertexFormat->getElementIndex(i));
		stream->writeUInt32(vertexFormat->getElementFormat(i));
	}
};

Material::ptr TMSHStreamer::readMaterial(DataStream *stream,int blockSize){
	if(stream->readBool()==false){
		return NULL;
	}

	Material::ptr material=new Material(mEngine->getMaterialManager());

	String name=stream->readNullTerminatedString();

	if(name!=(char*)NULL){
		material->setName(name);
	}

	RenderState::ptr renderState=material->getRenderState();

	if(stream->readBool()){
		MaterialState materialState;
		stream->read((tbyte*)&materialState,sizeof(MaterialState));
		if(renderState!=NULL){
			renderState->setMaterialState(materialState);
		}
	}

	if(stream->readBool()){
		RasterizerState rasterizerState;
		stream->read((tbyte*)&rasterizerState,sizeof(RasterizerState));
		if(renderState!=NULL){
			renderState->setRasterizerState(rasterizerState);
		}
	}

	if(stream->readBool()){
		BlendState blendState;
		stream->read((tbyte*)&blendState,sizeof(BlendState));
		if(renderState!=NULL){
			renderState->setBlendState(blendState);
		}
	}

	return material;
}

void TMSHStreamer::writeMaterial(DataStream *stream,Material::ptr material){
	if(material==NULL){
		stream->writeBool(false);
		return;
	}

	stream->writeBool(true);

	stream->writeNullTerminatedString(material->getName());

	RenderState::ptr renderState=material->getRenderState();

	MaterialState materialState;
	if(renderState!=NULL && renderState->getMaterialState(materialState)){
		stream->writeBool(true);
		stream->write((tbyte*)&materialState,sizeof(MaterialState));
	}
	else{
		stream->writeBool(false);
	}

	RasterizerState rasterizerState;
	if(renderState!=NULL && renderState->getRasterizerState(rasterizerState)){
		stream->writeBool(true);
		stream->write((tbyte*)&rasterizerState,sizeof(RasterizerState));
	}
	else{
		stream->writeBool(false);
	}

	BlendState blendState;
	if(renderState!=NULL && renderState->getBlendState(blendState)){
		stream->writeBool(true);
		stream->write((tbyte*)&blendState,sizeof(BlendState));
	}
	else{
		stream->writeBool(false);
	}
}

Skeleton::ptr TMSHStreamer::readSkeleton(DataStream *stream,int blockSize){
	Skeleton::ptr skeleton=new Skeleton();

	int numBones=stream->readInt32();

	int i;
	for(i=0;i<numBones;++i){
		Skeleton::Bone::ptr bone=new Skeleton::Bone();

		bone->index=stream->readInt32();
		bone->parentIndex=stream->readInt32();

		stream->readVector3(bone->translate);
		stream->readQuaternion(bone->rotate);
		stream->readVector3(bone->scale);

		stream->readVector3(bone->worldToBoneTranslate);
		stream->readQuaternion(bone->worldToBoneRotate);

		stream->readAABox(bone->bound);

		stream->readNullTerminatedString(bone->name);

		skeleton->addBone(bone);
	}

	return skeleton;
}

void TMSHStreamer::writeSkeleton(DataStream *stream,Skeleton::ptr skeleton){
	stream->writeInt32(skeleton->getNumBones());

	int i;
	for(i=0;i<skeleton->getNumBones();++i){
		Skeleton::Bone *bone=skeleton->getBone(i);
	
		stream->writeInt32(bone->index);
		stream->writeInt32(bone->parentIndex);

		stream->writeVector3(bone->translate);
		stream->writeQuaternion(bone->rotate);
		stream->writeVector3(bone->scale);

		stream->writeVector3(bone->worldToBoneTranslate);
		stream->writeQuaternion(bone->worldToBoneRotate);

		stream->writeAABox(bone->bound);

		stream->writeNullTerminatedString(bone->name);
	}
}

Sequence::ptr TMSHStreamer::readSequence(DataStream *stream,int blockSize){
	Sequence::ptr sequence=new Sequence();

	int numTracks=stream->readInt32();

	int i,j;
	for(i=0;i<numTracks;++i){
		Track::ptr track=new Track(mEngine->getVertexFormats().POSITION_ROTATE_SCALE);
		VertexBufferAccessor &vba=track->getAccessor();

		track->setIndex(stream->readInt32());
		track->setLength(stream->readFloat());

		int numKeyFrames=stream->readInt32();
		for(j=0;j<numKeyFrames;++j){
			int index=track->addKeyFrame(stream->readFloat());

			Vector3 translate;
			Quaternion rotate;
			Vector3 scale;
			stream->readVector3(translate);
			stream->readQuaternion(rotate);
			stream->readVector3(scale);
			vba.set3(index,0,translate);
			vba.set4(index,1,rotate);
			vba.set3(index,2,scale);
		}

		sequence->addTrack(track);
	}

	sequence->setLength(stream->readFloat());

	return sequence;
}

void TMSHStreamer::writeSequence(DataStream *stream,Sequence::ptr sequence){
	stream->writeInt32(sequence->getNumTracks());

	int i,j;
	for(i=0;i<sequence->getNumTracks();++i){
		Track *track=sequence->getTrack(i);
		VertexBufferAccessor &vba=track->getAccessor();

		stream->writeInt32(track->getIndex());
		stream->writeFloat(track->getLength());
		stream->writeInt32(track->getNumKeyFrames());

		for(j=0;j<track->getNumKeyFrames();++j){
			stream->writeFloat(track->getTime(j));

			Vector3 translate;
			Quaternion rotate;
			Vector3 scale;
			vba.get3(j,0,translate);
			vba.get4(j,1,rotate);
			vba.get3(j,2,scale);
			stream->writeVector3(translate);
			stream->writeQuaternion(rotate);
			stream->writeVector3(scale);
		}
	}

	stream->writeFloat(sequence->getLength());
}

void TMSHStreamer::MaterialRequest::request(){
	while(mIndex<mMesh->getNumSubMeshes() && mMesh->getSubMesh(mIndex)->materialName.length()!=0){
		mIndex++;
	}
	
	if(mIndex<mMesh->getNumSubMeshes()){
		mMaterialManager->find(mMesh->getSubMesh(mIndex)->material->getName(),this);
	}
	else{
		mMesh->compile();
		mRequest->resourceReady(mMesh);
	}
}

void TMSHStreamer::MaterialRequest::resourceReady(Resource *resource){
	Material::ptr material=(Material*)resource;
	Mesh::SubMesh *subMesh=mMesh->getSubMesh(mIndex);
	mMaterialManager->modifyRenderState(material->getRenderState(),subMesh->material->getRenderState());
	subMesh->material=material;

	mIndex++;
	request();
}

void TMSHStreamer::MaterialRequest::resourceException(const Exception &ex){
	mIndex++;
	request();
}

}
}
