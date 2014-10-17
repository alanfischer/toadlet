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

#include "AssimpHandler.h"
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>

namespace toadlet{
namespace tadpole{

AssimpHandler::AssimpHandler(Engine *engine):
	mEngine(engine)
{}

Node::ptr AssimpHandler::load(Stream *stream,const String &format){
	int length=stream->length();
	tbyte *data=new tbyte[length];
	stream->read(data,length);

	Assimp::Importer *importer=new Assimp::Importer();
	
	const aiScene *scene=importer->ReadFileFromMemory(data,length,0,format);
	
	Node::ptr node=loadScene(scene);
	
	delete importer;
	
	delete[] data;
	
	return node;
}

Node::ptr AssimpHandler::loadScene(const aiScene *ascene){
	int i;

	Scene *scene=new Scene();

	scene->cameras.resize(ascene->mNumCameras);

	scene->meshes.resize(ascene->mNumMeshes);
	for(i=0;i<ascene->mNumMeshes;++i){
		scene->meshes[i]=loadMesh(ascene->mMeshes[i]);
	}

	Node::ptr node=loadScene(scene,ascene,ascene->mRootNode);

	delete scene;

	return node;
}

Node::ptr AssimpHandler::loadScene(Scene *scene,const aiScene *ascene,const aiNode *anode){
	Node::ptr node=new Node();

	node->setName(anode->mName.C_Str());

	node->getTransform()->setMatrix(toMatrix4x4(anode->mTransformation));

	int i;
	for(i=0;i<anode->mNumMeshes;++i){
		MeshComponent::ptr meshComponent=new MeshComponent(mEngine);
		meshComponent->setMesh(scene->meshes[anode->mMeshes[i]]);
		node->attach(meshComponent);
	}

	for(i=0;i<anode->mNumChildren;++i){
		node->attach(loadScene(scene,ascene,anode->mChildren[i]));
	}

	return node;
}

Mesh::ptr AssimpHandler::loadMesh(const aiMesh *amesh){
	int i,v;

	VertexFormat::ptr format=mEngine->getBufferManager()->createVertexFormat();
	{
		if(amesh->HasPositions()){
			format->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_COUNT_3|VertexFormat::Format_TYPE_FLOAT_32);
		}
		if(amesh->HasNormals()){
			format->addElement(VertexFormat::Semantic_NORMAL,0,VertexFormat::Format_COUNT_3|VertexFormat::Format_TYPE_FLOAT_32);
		}
		for(i=0;i<amesh->GetNumColorChannels();++i){
			if(amesh->HasVertexColors(i)){
				format->addElement(VertexFormat::Semantic_COLOR,i,VertexFormat::Format_COUNT_4|VertexFormat::Format_TYPE_FLOAT_32);
			}
		}
		for(i=0;i<amesh->GetNumUVChannels();++i){
			if(amesh->HasTextureCoords(i)){
				int count=amesh->mNumUVComponents[i];
				format->addElement(VertexFormat::Semantic_TEXCOORD,i,(VertexFormat::Format_COUNT_1<<(count-1))|VertexFormat::Format_TYPE_FLOAT_32);
			}
		}
		if(amesh->HasTangentsAndBitangents()){
			format->addElement(VertexFormat::Semantic_TANGENT,i,VertexFormat::Format_COUNT_3|VertexFormat::Format_TYPE_FLOAT_32);
			format->addElement(VertexFormat::Semantic_BITANGENT,i,VertexFormat::Format_COUNT_3|VertexFormat::Format_TYPE_FLOAT_32);
		}
	}
	format->create();

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,format,amesh->mNumVertices);
	{
		int e=0;
		VertexBufferAccessor vba(vertexBuffer);
		if(amesh->HasPositions()){
			for(v=0;v<amesh->mNumVertices;++v){
				vba.set3(v,e,toVector3(amesh->mVertices[v]));
			}
			e++;
		}
		if(amesh->HasNormals()){
			for(v=0;v<amesh->mNumVertices;++v){
				vba.set3(v,e,toVector3(amesh->mNormals[v]));
			}
			e++;
		}
		for(i=0;i<amesh->GetNumColorChannels();++i){
			if(amesh->HasVertexColors(i)){
				for(v=0;v<amesh->mNumVertices;++v){
					vba.set4(v,e,toColor4(amesh->mColors[i][v]));
				}
				e++;
			}
		}
		for(i=0;i<amesh->GetNumUVChannels();++i){
			if(amesh->HasTextureCoords(i)){
				int count=amesh->mNumUVComponents[i];
				switch(count){
					case 1:
						for(v=0;v<amesh->mNumVertices;++v){
							vba.set(v,e,amesh->mTextureCoords[i][v].x);
						}
					break;
					case 2:
						for(v=0;v<amesh->mNumVertices;++v){
							vba.set2(v,e,amesh->mTextureCoords[i][v].x,amesh->mTextureCoords[i][v].y);
						}
					break;
					case 3:
						for(v=0;v<amesh->mNumVertices;++v){
							vba.set3(v,e,toVector3(amesh->mTextureCoords[i][v]));
						}
					break;
				}
				e++;
			}
		}
		if(amesh->HasTangentsAndBitangents()){
			for(v=0;v<amesh->mNumVertices;++v){
				vba.set3(v,e,toVector3(amesh->mTangents[v]));
				vba.set3(v,e+1,toVector3(amesh->mBitangents[v]));
			}
			e+=2;
		}
		vba.unlock();
	}

	IndexBuffer::ptr indexBuffer;
	if(amesh->HasFaces()){
		int numIndexes=0;
		for(i=0;i<amesh->mNumFaces;++i){
			numIndexes+=amesh->mFaces[i].mNumIndices;
		}
		indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,numIndexes);

		int e=0;
		IndexBufferAccessor iba(indexBuffer);
		for(i=0;i<amesh->mNumFaces;++i){
			for(v=0;v<amesh->mFaces[i].mNumIndices;++v){
				iba.set(e,amesh->mFaces[i].mIndices[v]);
				e++;
			}
		}
		iba.unlock();
	}

	Mesh::ptr mesh=new Mesh();

	mesh->setStaticVertexData(new VertexData(vertexBuffer));

	Mesh::SubMesh::ptr subMesh=new Mesh::SubMesh();
	subMesh->indexData=new IndexData(IndexData::Primitive_TRIS,indexBuffer);
	subMesh->material=mEngine->createDiffuseMaterial(NULL);

	mesh->addSubMesh(subMesh);
	mesh->setBound(new Bound(Bound::Type_INFINITE));

	return mesh;
}

bool AssimpHandler::save(Stream *stream,Node *node,const String &format){
	const aiScene *scene=saveScene(node);

	Assimp::Exporter *exporter=new Assimp::Exporter();
	
	const aiExportDataBlob *blob=exporter->ExportToBlob(scene,format);

	while(blob!=NULL){
		stream->write((tbyte*)blob->data,blob->size);
		blob=blob->next;
	}
	
	delete exporter;
	
	delete scene;

	return true;
}

const aiScene *AssimpHandler::saveScene(Node *node){
	Scene *scene=new Scene();

	aiScene *ascene=new aiScene();

	ascene->mRootNode=saveScene(scene,ascene,node);

	delete scene;

	return ascene;
}

aiNode *AssimpHandler::saveScene(Scene *scene,const aiScene *ascene,Node *node){
	aiNode *anode=new aiNode();

	anode->mName=node->getName();

	anode->mTransformation=toMatrix4x4(node->getTransform()->getMatrix());

	int i;
	MeshComponent::ptr mesh=node->getChildType<MeshComponent>();
	if(mesh!=NULL){
		// TODO
	}

	tforeach(NodeRange::iterator,child,node->getNodes()){anode->mNumChildren++;}

	if(anode->mNumChildren>0){
		anode->mChildren=new aiNode*[anode->mNumChildren];
		i=0;
		tforeach(NodeRange::iterator,child,node->getNodes()){
			anode->mChildren[i++]=saveScene(scene,ascene,child);
		}
	}

	return anode;
}

}
}
