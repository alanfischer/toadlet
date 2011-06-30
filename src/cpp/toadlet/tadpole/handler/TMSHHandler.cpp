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
#include <toadlet/tadpole/handler/TMSHHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

TMSHHandler::TMSHHandler(Engine *engine){
	mBufferManager=engine->getBufferManager();
	mMaterialManager=engine->getMaterialManager();
	mEngine=engine;
}

Resource::ptr TMSHHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	int i,j;

	DataStream::ptr dataStream(new DataStream(stream));

	int id=dataStream->readUInt32();
	if(id!=TMSH){
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
	Collection<Material::ptr> materials;
	Skeleton::ptr skeleton;
	Collection<TransformSequence::ptr> sequences;
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

	j=0;
	for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh *subMesh=mesh->getSubMesh(i);
		if(subMesh->materialName.length()==0 && j<materials.size()){
			subMesh->material=materials[j++];
			if(subMesh->material!=NULL){
				subMesh->material->retain();
			}
		}
	}

	if(skeleton!=NULL){
		for(i=0;i<sequences.size();++i){
			skeleton->sequences.add(sequences[i]);
		}
	}

	mesh->setSkeleton(skeleton);

	return mesh;
}

bool TMSHHandler::save(Mesh::ptr mesh,Stream::ptr stream){
	int i;

	DataStream::ptr dataStream(new DataStream(stream));

	dataStream->writeUInt32(TMSH);

	dataStream->writeUInt32(VERSION);

	MemoryStream::ptr memoryStream(new MemoryStream());
	DataStream::ptr dataMemoryStream(new DataStream(shared_static_cast<Stream>(memoryStream)));

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

		for(i=0;i<skeleton->sequences.size();++i){
			TransformSequence::ptr sequence=skeleton->sequences[i];
			writeSequence(dataMemoryStream,sequence);
			dataStream->writeInt32(Block_SEQUENCE);
			dataStream->writeInt32(memoryStream->length());
			dataStream->write(memoryStream->getOriginalDataPointer(),memoryStream->length());
			memoryStream->reset();
		}
	}

	return true;
}

Mesh::ptr TMSHHandler::readMesh(DataStream *stream,int blockSize){
	int i;

	Mesh::ptr mesh(new Mesh());
	
	Transform transform;
	stream->read((tbyte*)&transform,sizeof(Transform));
	mesh->setTransform(transform);

	Bound bound;
	stream->read((tbyte*)&bound,sizeof(Bound));
	mesh->setBound(bound);

	int numSubMeshes=stream->readUInt32();
	for(i=0;i<numSubMeshes;++i){
		Mesh::SubMesh::ptr subMesh(new Mesh::SubMesh());
	
		subMesh->vertexData=readVertexData(stream);
		subMesh->indexData=readIndexData(stream);

		subMesh->materialName=stream->readNullTerminatedString();
		
		subMesh->name=stream->readNullTerminatedString();

		subMesh->hasOwnTransform=stream->readBool();
		stream->read((tbyte*)&subMesh->transform,sizeof(Transform));
		stream->read((tbyte*)&subMesh->bound,sizeof(Bound));

		mesh->addSubMesh(subMesh);
	}
	mesh->setStaticVertexData(readVertexData(stream));

	Collection<Mesh::VertexBoneAssignmentList> vbas;
	vbas.resize(stream->readUInt32());
	for(i=0;i<vbas.size();++i){
		Mesh::VertexBoneAssignmentList &list=vbas[i];
		
		int numAssignments=stream->readUInt8();
		list.resize(numAssignments);
		stream->read((tbyte*)list.begin(),numAssignments*sizeof(Mesh::VertexBoneAssignment));
	}
	mesh->setVertexBoneAssignments(vbas);

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


void TMSHHandler::writeMesh(DataStream *stream,Mesh::ptr mesh){
	stream->write((tbyte*)&mesh->getTransform(),sizeof(Transform));
	stream->write((tbyte*)&mesh->getBound(),sizeof(Bound));
	
	stream->writeUInt32(mesh->getNumSubMeshes());
	int i;
	for(i=0;i<mesh->getNumSubMeshes();++i){
		Mesh::SubMesh::ptr subMesh=mesh->getSubMesh(i);

		writeVertexData(stream,subMesh->vertexData);
		writeIndexData(stream,subMesh->indexData);

		stream->writeNullTerminatedString(subMesh->materialName);

		stream->writeNullTerminatedString(subMesh->name);

		stream->writeBool(subMesh->hasOwnTransform);
		stream->write((tbyte*)&subMesh->transform,sizeof(Transform));
		stream->write((tbyte*)&subMesh->bound,sizeof(Bound));
	}
	writeVertexData(stream,mesh->getStaticVertexData());

	const Collection<Mesh::VertexBoneAssignmentList> &vbas=mesh->getVertexBoneAssignments();
	stream->writeUInt32(vbas.size());
	for(i=0;i<vbas.size();++i){
		const Mesh::VertexBoneAssignmentList &list=vbas[i];
		int numAssignments=list.size();
		stream->writeUInt8(numAssignments);
		stream->write((tbyte*)list.begin(),numAssignments*sizeof(Mesh::VertexBoneAssignment));
	}
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

Material::ptr TMSHHandler::readMaterial(DataStream *stream,int blockSize){
	int i;

	if(stream->readBool()==false){
		return NULL;
	}

	Material::ptr material=mEngine->getMaterialManager()->createMaterial();

	material->setName(stream->readNullTerminatedString());

	BlendState blendState;
	stream->read((tbyte*)&blendState,sizeof(blendState));
	material->setBlendState(blendState);

	DepthState depthState;
	stream->read((tbyte*)&depthState,sizeof(depthState));
	material->setDepthState(depthState);

	RasterizerState rasterizerState;
	stream->read((tbyte*)&rasterizerState,sizeof(rasterizerState));
	material->setRasterizerState(rasterizerState);

	FogState fogState;
	stream->read((tbyte*)&fogState,sizeof(fogState));
	material->setFogState(fogState);

	PointState pointState;
	stream->read((tbyte*)&pointState,sizeof(pointState));
	material->setPointState(pointState);

	MaterialState materialState;
	stream->read((tbyte*)&materialState,sizeof(materialState));
	material->setMaterialState(materialState);

	int numSamplerStates=stream->readInt32();
	for(i=0;i<numSamplerStates;++i){
		SamplerState samplerState;
		stream->read((tbyte*)&samplerState,sizeof(samplerState));
		material->setSamplerState(i,samplerState);
	}

	int numTextureStates=stream->readInt32();
	for(i=0;i<numTextureStates;++i){
		TextureState textureState;
		stream->read((tbyte*)&textureState,sizeof(textureState));
		material->setTextureState(i,textureState);
	}

	int numTextures=stream->readInt32();
	for(i=0;i<numTextures;++i){
		String textureName=stream->readNullTerminatedString();
		material->setTexture(i,mEngine->getTextureManager()->findTexture(textureName));
		material->setTextureName(i,textureName);
	}

	return material;
}

void TMSHHandler::writeMaterial(DataStream *stream,Material::ptr material){
	int i;

	if(material==NULL){
		stream->writeBool(false);
		return;
	}

	stream->writeBool(true);

	stream->writeNullTerminatedString(material->getName());

	BlendState blendState;
	material->getBlendState(blendState);
	stream->write((tbyte*)&blendState,sizeof(blendState));

	DepthState depthState;
	material->getDepthState(depthState);
	stream->write((tbyte*)&depthState,sizeof(depthState));

	RasterizerState rasterizerState;
	material->getRasterizerState(rasterizerState);
	stream->write((tbyte*)&rasterizerState,sizeof(rasterizerState));

	FogState fogState;
	material->getFogState(fogState);
	stream->write((tbyte*)&fogState,sizeof(fogState));

	PointState pointState;
	material->getPointState(pointState);
	stream->write((tbyte*)&pointState,sizeof(pointState));

	MaterialState materialState;
	material->getMaterialState(materialState);
	stream->write((tbyte*)&materialState,sizeof(materialState));

	stream->writeInt32(material->getNumSamplerStates());
	for(i=0;i<material->getNumSamplerStates();++i){
		SamplerState samplerState;
		material->getSamplerState(i,samplerState);
		stream->write((tbyte*)&samplerState,sizeof(samplerState));
	}

	stream->writeInt32(material->getNumTextureStates());
	for(i=0;i<material->getNumTextureStates();++i){
		TextureState textureState;
		material->getTextureState(i,textureState);
		stream->write((tbyte*)&textureState,sizeof(textureState));
	}

	stream->writeInt32(material->getNumTextures());
	for(i=0;i<material->getNumTextures();++i){
		Texture::ptr texture=material->getTexture(i);
		stream->writeNullTerminatedString(texture!=NULL?texture->getName():material->getTextureName(i));
	}
}

Skeleton::ptr TMSHHandler::readSkeleton(DataStream *stream,int blockSize){
	Skeleton::ptr skeleton(new Skeleton());

	skeleton->bones.resize(stream->readInt32());

	int i;
	for(i=0;i<skeleton->bones.size();++i){
		Skeleton::Bone::ptr bone(new Skeleton::Bone());

		bone->index=stream->readInt32();
		bone->parentIndex=stream->readInt32();

		stream->readVector3(bone->translate);
		stream->readQuaternion(bone->rotate);
		stream->readVector3(bone->scale);

		stream->readVector3(bone->worldToBoneTranslate);
		stream->readQuaternion(bone->worldToBoneRotate);

		stream->readAABox(bone->bound);

		stream->readNullTerminatedString(bone->name);

		skeleton->bones[i]=bone;
	}

	return skeleton;
}

void TMSHHandler::writeSkeleton(DataStream *stream,Skeleton::ptr skeleton){
	stream->writeInt32(skeleton->bones.size());

	int i;
	for(i=0;i<skeleton->bones.size();++i){
		Skeleton::Bone *bone=skeleton->bones[i];

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

TransformSequence::ptr TMSHHandler::readSequence(egg::io::DataStream *stream,int blockSize){
	TransformSequence::ptr sequence(new TransformSequence());

	sequence->tracks.resize(stream->readInt32());

	int i,j;
	for(i=0;i<sequence->tracks.size();++i){
		TransformTrack::ptr track(new TransformTrack());
		sequence->tracks[i]=track;

		track->index=stream->readInt32();
		track->length=stream->readFloat();
		track->keyFrames.resize(stream->readInt32());

		for(j=0;j<track->keyFrames.size();++j){
			TransformKeyFrame frame;
			frame.time=stream->readFloat();
			stream->readVector3(frame.translate);
			stream->readQuaternion(frame.rotate);
			stream->readVector3(frame.scale);
			track->keyFrames[j]=frame;
		}
	}

	sequence->length=stream->readFloat();
	sequence->hasScale=stream->readBool();

	return sequence;
}

void TMSHHandler::writeSequence(egg::io::DataStream *stream,TransformSequence::ptr sequence){
	stream->writeInt32(sequence->tracks.size());

	int i,j;
	for(i=0;i<sequence->tracks.size();++i){
		TransformTrack *track=sequence->tracks[i];

		stream->writeInt32(track->index);
		stream->writeFloat(track->length);
		stream->writeInt32(track->keyFrames.size());

		for(j=0;j<track->keyFrames.size();++j){
			const TransformKeyFrame &frame=track->keyFrames[j];
			stream->writeFloat(frame.time);
			stream->writeVector3(frame.translate);
			stream->writeQuaternion(frame.rotate);
			stream->writeVector3(frame.scale);
		}
	}

	stream->writeFloat(sequence->length);
	stream->writeBool(sequence->hasScale);
}

}
}
}
