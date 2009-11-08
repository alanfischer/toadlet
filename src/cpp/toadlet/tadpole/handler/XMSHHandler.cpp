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
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/handler/XMSHHandler.h>
#include <toadlet/tadpole/handler/XMLMeshUtilities.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{
namespace handler{

XMSHHandler::XMSHHandler(BufferManager *bufferManager,ResourceManager *materialManager,TextureManager *textureManager){
	mBufferManager=bufferManager;
	mMaterialManager=materialManager;
	mTextureManager=textureManager;
}

Resource::ptr XMSHHandler::load(InputStream::ptr in,const ResourceHandlerData *handlerData){
	Mesh::ptr mesh;

	// TODO: Replace the following when mxml implements custom load/save callbacks
	char buffer[1025];
	int amount=0;
	String string;
	while((amount=in->read(buffer,1024))>0){
		buffer[amount]=0;
		string+=buffer;
	}

	mxml_node_t *root=mxmlLoadString(NULL,string,MXML_OPAQUE_CALLBACK);

	if(strcmp(mxmlGetElementName(root),"XMSH")==0){
		int version=0;
		const char *versionProp=mxmlElementGetAttr(root,"Version");
		if(versionProp!=NULL){
			version=XMLMeshUtilities::parseInt(versionProp);
		}

		if(version==1){
			mesh=loadMeshVersion1(root);
		}
		else if(version>=2){
			mesh=loadMeshVersion2Up(root,version);
		}
		else{
			mxmlRelease(root);
			Error::unknown(Categories::TOADLET_TADPOLE,
				"XMSHHandler: Invalid version");
			return NULL;
		}
	}

	mxmlRelease(root);

	return mesh;
}

bool XMSHHandler::save(Mesh::ptr mesh,OutputStream::ptr out){
	mxml_node_t *root=mxmlNewElement(MXML_NO_PARENT,"XMSH");

	int version=XMLMeshUtilities::version;
	mxmlElementSetAttr(root,"Version",XMLMeshUtilities::makeInt(version));

	if(version==1){
		saveMeshVersion1(root,mesh);
	}
	else if(version>=2){
		saveMeshVersion2Up(root,mesh,version);
	}
	else{
		mxmlRelease(root);
		Error::unknown(Categories::TOADLET_TADPOLE,
			"XMSHHandler: Invalid version");
		return false;
	}

	char *string=mxmlSaveAllocString(root,XMLMeshUtilities::mxmlSaveCallback);
	out->write(string,strlen(string));
	free(string);

	mxmlRelease(root);

	return true;
}

Mesh::ptr XMSHHandler::loadMeshVersion1(mxml_node_t *root){
	Mesh::ptr mesh=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"MeshData")==0){
			mesh=XMLMeshUtilities::loadMesh(block,1,mBufferManager,mMaterialManager,mTextureManager);
		}
		else if(strcmp(mxmlGetElementName(block),"SkeletonData")==0){
			mesh->skeleton=Skeleton::ptr(XMLMeshUtilities::loadSkeleton(block,1));
		}
		else if(strcmp(mxmlGetElementName(block),"AnimationData")==0){
			mesh->skeleton->sequences.add(Sequence::ptr(XMLMeshUtilities::loadSequence(block,1)));
		}
	}

	return mesh;
}

Mesh::ptr XMSHHandler::loadMeshVersion2Up(mxml_node_t *root,int version){
	Mesh::ptr mesh=NULL;

	mxml_node_t *block=root->child;
	while((block=block->next)!=NULL){
		if(strcmp(mxmlGetElementName(block),"Mesh")==0){
			mesh=XMLMeshUtilities::loadMesh(block,version,mBufferManager,mMaterialManager,mTextureManager);
		}
		else if(strcmp(mxmlGetElementName(block),"Skeleton")==0){
			mesh->skeleton=Skeleton::ptr(XMLMeshUtilities::loadSkeleton(block,version));
		}
		else if(strcmp(mxmlGetElementName(block),"Sequence")==0){
			mesh->skeleton->sequences.add(Sequence::ptr(XMLMeshUtilities::loadSequence(block,version)));
		}
	}

	return mesh;
}

bool XMSHHandler::saveMeshVersion1(mxml_node_t *root,Mesh::ptr mesh){
	if(mesh!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMesh(mesh,1);
		mxmlSetElement(node,"MeshData");
		mxmlAddChild(root,node);
	}
	if(mesh!=NULL && mesh->skeleton!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSkeleton(mesh->skeleton,1);
		mxmlSetElement(node,"SkeletonData");
		mxmlAddChild(root,node);

		int i;
		for(i=0;i<mesh->skeleton->sequences.size();++i){
			Sequence::ptr sequence=mesh->skeleton->sequences[i];
			mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,1);
			mxmlSetElement(node,"AnimationData");
			mxmlAddChild(root,node);
		}
	}

	return true;
}

bool XMSHHandler::saveMeshVersion2Up(mxml_node_t *root,Mesh::ptr mesh,int version){
	if(mesh!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveMesh(mesh,version);
		mxmlSetElement(node,"Mesh");
		mxmlAddChild(root,node);
	}
	if(mesh!=NULL && mesh->skeleton!=NULL){
		mxml_node_t *node=XMLMeshUtilities::saveSkeleton(mesh->skeleton,version);
		mxmlSetElement(node,"Skeleton");
		mxmlAddChild(root,node);

		int i;
		for(i=0;i<mesh->skeleton->sequences.size();++i){
			Sequence::ptr sequence=mesh->skeleton->sequences[i];
			mxml_node_t *node=XMLMeshUtilities::saveSequence(sequence,version);
			mxmlSetElement(node,"Sequence");
			mxmlAddChild(root,node);
		}
	}

	return true;
}

}
}
}
