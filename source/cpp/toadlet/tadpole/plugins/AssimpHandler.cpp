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
	
	Node::ptr node=loadScene(mEngine,scene);
	
	delete importer;
	
	delete[] data;
	
	return node;
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

Node::ptr AssimpHandler::loadScene(Engine *engine,const aiScene *ascene){
	int i;

	Scene *scene=new Scene();

	scene->cameras.resize(ascene->mNumCameras);

	scene->textures.resize(ascene->mNumTextures);

	scene->materials.resize(ascene->mNumMaterials);
	for(i=0;i<ascene->mNumMaterials;++i){
		scene->materials[i]=loadMaterial(engine,scene,ascene->mMaterials[i]);
	}

	scene->meshes.resize(ascene->mNumMeshes);
	for(i=0;i<ascene->mNumMeshes;++i){
		scene->meshes[i]=loadMesh(engine,scene,ascene->mMeshes[i]);
	}

	Node::ptr node=loadScene(engine,scene,ascene,ascene->mRootNode);

	delete scene;

	return node;
}

Node::ptr AssimpHandler::loadScene(Engine *engine,Scene *scene,const aiScene *ascene,const aiNode *anode){
	Node::ptr node=new Node();

	node->setName(anode->mName.C_Str());

	node->getTransform()->setMatrix(toMatrix4x4(anode->mTransformation));

	int i;
	for(i=0;i<anode->mNumMeshes;++i){
		MeshComponent::ptr meshComponent=new MeshComponent(engine);
		meshComponent->setMesh(scene->meshes[anode->mMeshes[i]]);
		node->attach(meshComponent);
	}

	for(i=0;i<anode->mNumChildren;++i){
		node->attach(loadScene(engine,scene,ascene,anode->mChildren[i]));
	}

	return node;
}

aiScene *AssimpHandler::saveScene(const Node *node){
	Scene *scene=new Scene();

	aiScene *ascene=new aiScene();

	ascene->mRootNode=saveScene(scene,ascene,node);

	ascene->mNumMeshes=scene->meshes.size();
	for(int i=0;i<ascene->mNumMeshes;++i){
		ascene->mMeshes[i]=saveMesh(scene,scene->meshes[i]);
	}

	ascene->mNumMaterials=scene->materials.size();
	for(int i=0;i<ascene->mNumMaterials;++i){
		ascene->mMaterials[i]=saveMaterial(scene,scene->materials[i]);
	}

	delete scene;

	return ascene;
}

aiNode *AssimpHandler::saveScene(Scene *scene,const aiScene *ascene,const Node *node){
	aiNode *anode=new aiNode();

	anode->mName=node->getName();

	anode->mTransformation=toMatrix4x4(node->getTransform()->getMatrix());

	Collection<MeshComponent::ptr> meshes;
	tforeach(ComponentRange::iterator,component,node->getComponents()){
		if(component->getType()==MeshComponent::type()) meshes.push_back(static_cast<MeshComponent*>(*component));
	}

	anode->mNumMeshes=std::distance(meshes.begin(),meshes.end());

	if(anode->mNumMeshes>0){
		anode->mMeshes=new unsigned int[anode->mNumMeshes];
		int i=0;
		tforeach(Collection<MeshComponent::ptr>::iterator,it,meshes){
			anode->mMeshes[i++]=scene->meshes.size();
			scene->meshes.push_back((*it)->getMesh());
		}
	}

	NodeRange children=node->getNodes();
	anode->mNumChildren=std::distance(children.begin(),children.end());

	if(anode->mNumChildren>0){
		anode->mChildren=new aiNode*[anode->mNumChildren];
		int i=0;
		tforeach(NodeRange::iterator,child,node->getNodes()){
			anode->mChildren[i++]=saveScene(scene,ascene,child);
		}
	}

	return anode;
}

Mesh::ptr AssimpHandler::loadMesh(Engine *engine,Scene *scene,const aiMesh *amesh){
	int i,v;

	VertexFormat::ptr format=engine->getBufferManager()->createVertexFormat();
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

	VertexBuffer::ptr vertexBuffer=engine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,format,amesh->mNumVertices);
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
		indexBuffer=engine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,numIndexes);

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
	subMesh->material=scene->materials[amesh->mMaterialIndex];

	mesh->addSubMesh(subMesh);
	mesh->setBound(new Bound(Bound::Type_INFINITE));

	return mesh;
}

aiMesh *AssimpHandler::saveMesh(Scene *scene,const Mesh *mesh){
	int i,j;

	aiMesh *amesh=new aiMesh();

	VertexBuffer::ptr vertexBuffer=mesh->getStaticVertexData()->getVertexBuffer(0);
	amesh->mNumVertices=vertexBuffer->getSize();
	VertexFormat::ptr format=vertexBuffer->getVertexFormat();
	{
		VertexBufferAccessor vba(vertexBuffer);
		int vi=format->findElement(VertexFormat::Semantic_POSITION);
		if(vi>=0){
			for(i=0;i<amesh->mNumVertices;++i){
				Vector3 v;
				vba.get3(i,vi,v);
				amesh->mVertices[i]=toVector3(v);
			}
		}

		int ni=format->findElement(VertexFormat::Semantic_NORMAL);
		if(ni>=0){
			for(i=0;i<amesh->mNumVertices;++i){
				Vector3 n;
				vba.get3(i,ni,n);
				amesh->mNormals[i]=toVector3(n);
			}
		}

		int k=0;
		for(j=0;j<format->getNumElements();++j){
			if(format->getElementSemantic(j)==VertexFormat::Semantic_TEXCOORD){
				amesh->mNumUVComponents[k++]=(format->getElementFormat(j)&VertexFormat::Format_MASK_COUNTS)>>VertexFormat::Format_SHIFT_COUNTS;
				for(i=0;i<amesh->mNumVertices;++i){
					Vector3 t;
					vba.get3(i,j,t);
					amesh->mTextureCoords[i][k]=toVector3(t);
				}
			}
		}

		int ti=format->findElement(VertexFormat::Semantic_TANGENT);
		int bi=format->findElement(VertexFormat::Semantic_BITANGENT);
		if(ti>=0 && bi>=0){
			for(i=0;i<amesh->mNumVertices;++i){
				Vector3 t,b;
				vba.get3(i,ti,t);
				vba.get3(i,bi,b);
				amesh->mTangents[i]=toVector3(t);
				amesh->mBitangents[i]=toVector3(b);
			}
		}
		vba.unlock();
	}

	if(mesh->getNumSubMeshes()>0){
		Mesh::SubMesh::ptr subMesh=mesh->getSubMesh(0);
		IndexData::ptr indexData=subMesh->indexData;
		IndexBuffer::ptr indexBuffer=indexData->getIndexBuffer();

		IndexBufferAccessor iba(indexBuffer);
		if(indexData->getPrimitive()==IndexData::Primitive_TRIS){
			amesh->mNumFaces=indexData->getCount()/3;
			amesh->mFaces=new aiFace[amesh->mNumFaces];
			for(i=0;i<amesh->mNumFaces;++i){
				aiFace *face=&amesh->mFaces[i];
				face->mNumIndices=3;
				face->mIndices=new unsigned int[3];

				face->mIndices[0]=iba.get(indexData->getStart()+i*3+0);
				face->mIndices[1]=iba.get(indexData->getStart()+i*3+1);
				face->mIndices[2]=iba.get(indexData->getStart()+i*3+2);
			}
		}
		iba.unlock();

		for(i=0;i<scene->materials.size();++i){
			if(scene->materials[i]==subMesh->material){
				break;
			}
		}
		if(i==scene->materials.size()){
			scene->materials.push_back(subMesh->material);
		}
		amesh->mMaterialIndex=i;
	}

	return amesh;
}

Material::ptr AssimpHandler::loadMaterial(Engine *engine,Scene *scene,const aiMaterial *amaterial){
	Texture::ptr texture;

	if(amaterial->GetTextureCount(aiTextureType_DIFFUSE)>0){
		aiString path;
		aiTextureMapping mapping=aiTextureMapping_UV;
		unsigned int uvindex=0;
		amaterial->GetTexture(aiTextureType_DIFFUSE,0,&path,&mapping,&uvindex);

		if(path.length>0 && path.data[0]=='*'){
			unsigned int index=atoi(path.data+1);
			texture=scene->textures[index];
		}
		else{
			texture=static_pointer_cast<Texture>(engine->getTextureManager()->find(path.C_Str()));
		}
	}

	Material::ptr material=engine->createDiffuseMaterial(texture);

	aiString name;
	amaterial->Get(AI_MATKEY_NAME,name);
	material->setName(name.C_Str());

	return material;
}

aiMaterial *AssimpHandler::saveMaterial(Scene *scene,const Material *material){
	aiMaterial *amaterial=new aiMaterial();

	aiString name(std::string(material->getName()));
	amaterial->AddProperty(&name,AI_MATKEY_NAME);

	if(material->getPass()->getNumTextures(Shader::ShaderType_FRAGMENT)>0){
		Texture::ptr texture=material->getPass()->getTexture(Shader::ShaderType_FRAGMENT,0);
		aiString textureName(std::string(texture->getName()));
		amaterial->AddProperty(&textureName,AI_MATKEY_TEXTURE_DIFFUSE(0));
	}

	return amaterial;
}

}
}
